#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/cursorfont.h>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <dirent.h>
#include "binarizewolfjolion.cc"
#include "fileExists.cc"
#include <fstream>

#define SSTR( x ) dynamic_cast< std::ostringstream & >( \
        ( std::ostringstream() << std::dec << x ) ).str()

int preprocessing( Mat & sourceImage, Mat & processed)
{
	int winx=0, winy=0;
	float optK=0.5;
	NiblackVersion versionCode=WOLFJOLION;
	Mat grayImage;
	
	/// Resize gray image for better thresholding and recognition
	resize(sourceImage, grayImage, Size(), 2, 2, INTER_CUBIC);
	
	/// Convert the image to Gray
	cvtColor( grayImage, grayImage, CV_RGB2GRAY );
	
	/// Treat the window size
	if (winx==0||winy==0) {
		winy = (int) (2.0 * grayImage.rows-1)/3;
		winx = (int) grayImage.cols-1 < winy ? grayImage.cols-1 : winy;
		/// if the window is too big, than we asume that the image
		/// is not a single text box, but a document page: set
		/// the window size to a fixed constant.
		if (winx > 100)
		    winx = winy = 40;
	}
	
	Mat binImage (grayImage.rows, grayImage.cols, CV_8U);
	/// Threshold resized image
	NiblackSauvolaWolfJolion(grayImage, binImage, versionCode, winx, winy, optK, 128);
	
	copyMakeBorder(binImage, processed, 5, 5, 5, 5, BORDER_CONSTANT, Scalar(255, 255, 255));
	
	return 0;
}

int saveShred( const vector<Rect> & boundLineRect, const Mat & processed, const char* textFile )
{
	vector<std::string> text;
	std::string line;
	std::ifstream fin;
	vector<int> cvPNG;
	cvPNG.push_back(CV_IMWRITE_PNG_COMPRESSION);
	cvPNG.push_back(0);
	
	fin.open(textFile);
	if(fin.is_open()){
		while (std::getline(fin, line))
			text.push_back(line);
	}
	fin.close();
	
	if( boundLineRect.size() == text.size() ){
		std::ofstream fout;
		std::string parentName = textFile;
		/// Remove directory if present.
		/// Do this before extension removal incase directory has a period character.
		const size_t last_slash_idx = parentName.find_last_of("\\/");
		if (std::string::npos != last_slash_idx)
		{
		    parentName.erase(0, last_slash_idx + 1);
		}

		/// Remove extension if present.
		const size_t period_idx = parentName.rfind('.');
		if (std::string::npos != period_idx)
		{
		    parentName.erase(period_idx);
		}
		const std::string folder = "./data/book/shredded/" + parentName;
		mkdir(folder.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
		for(int i=0; i<text.size(); i++)
		{
			const std::string textName = "./data/book/shredded/" + parentName + "/" + SSTR(i) + ".gt.txt";
			const std::string imgName = "./data/book/shredded/" + parentName + "/" + SSTR(i) + ".bin.png";
			fout.open(textName.c_str());
			fout << text[i];
			fout.close();
			imwrite(imgName, processed(boundLineRect[i]), cvPNG);
		}
	} else {
		printf("Mismatch string count: recognized %i string on image but %i in file\n", boundLineRect.size(), text.size());
		return EXIT_FAILURE;
	}
	
	return 0;
}

int shredder( vector<Rect> & rectBoundStr, Mat & processed )
{
	Mat binImage;
	processed.copyTo(binImage);
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;
	
	if( !rectBoundStr.empty() )
		rectBoundStr.erase(rectBoundStr.begin(), rectBoundStr.end());
	
	/// Find contours
	findContours( binImage, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_TC89_L1, Point(0, 0) );
	
	/// Approximate contours to polygons + get bounding rects and circles
	vector<vector<Point> > contours_poly( contours.size() );
	vector<Rect> boundRect( contours.size() );
	vector<Rect> boundLineRect;
	
	for( int i = 0; i < contours.size(); i++ )
	{ 	
		approxPolyDP( Mat(contours[i]), contours_poly[i], 2, true );
		boundRect[i] = boundingRect( Mat(contours_poly[i]) );
	}
	
	//Scalar blu = Scalar( 237, 27, 27 );
	//Scalar red = Scalar( 27, 27, 237 );
	//Mat tmp;
	
	//processed.copyTo(tmp);
	boundLineRect.insert(boundLineRect.end(), boundRect[1]);
	//namedWindow("Display window", WINDOW_AUTOSIZE);
	for( int i=1; i<boundRect.size(); i++ )
	{
		//tmp.copyTo(processed);
		bool fit = false;
		vector<int> fitCount;
		unsigned int numberLines = boundLineRect.size();
		//rectangle( processed, boundRect[i].tl(), boundRect[i].br(), blu, 1, 8, 0 );
		for( int j=0; j<numberLines; j++ )
		{
			if( boundLineRect[j].y <= boundRect[i].y and boundLineRect[j].y + boundLineRect[j].height >= boundRect[i].y + boundRect[i].height ) {
				if( boundLineRect[j].x > boundRect[i].x ){
					boundLineRect[j].width = boundLineRect[j].br().x - boundRect[i].x;
					boundLineRect[j].x = boundRect[i].x;
				} else if( boundLineRect[j].x + boundLineRect[j].width < boundRect[i].x + boundRect[i].width ){
					boundLineRect[j].width = boundRect[i].x + boundRect[i].width - boundLineRect[j].x;
				}
				fit = true;
				fitCount.push_back(j);
			} else if( boundLineRect[j].y >= boundRect[i].y and boundLineRect[j].y + boundLineRect[j].height <= boundRect[i].y + boundRect[i].height ) {
				if( boundLineRect[j].x > boundRect[i].x ){
					boundLineRect[j].width = boundLineRect[j].br().x - boundRect[i].x;
					boundLineRect[j].x = boundRect[i].x;
				} else if( boundLineRect[j].x + boundLineRect[j].width < boundRect[i].x + boundRect[i].width ) {
					boundLineRect[j].width = boundRect[i].x + boundRect[i].width - boundLineRect[j].x;

				}
				boundLineRect[j].y = boundRect[i].y;
				boundLineRect[j].height = boundRect[i].height;
				fit = true;
				fitCount.push_back(j);
			} else if( boundLineRect[j].y < boundRect[i].y and boundLineRect[j].br().y > boundRect[i].y ) {
				float d1 = (float)(boundLineRect[j].br().y - boundRect[i].y)/boundLineRect[j].height;
				float d2 = (float)(boundLineRect[j].br().y - boundRect[i].y)/boundRect[i].height;
				if ( d1>0.5 or d2>0.5 ) {
					if( boundLineRect[j].x > boundRect[i].x ){
						boundLineRect[j].width = boundLineRect[j].br().x - boundRect[i].x;
						boundLineRect[j].x = boundRect[i].x;
					} else if( boundLineRect[j].x + boundLineRect[j].width < boundRect[i].x + boundRect[i].width ) {
						boundLineRect[j].width = boundRect[i].x + boundRect[i].width - boundLineRect[j].x;

					}
					boundLineRect[j].height = boundRect[i].br().y - boundLineRect[j].y;
					fit = true;
					fitCount.push_back(j);
				}
			} else if( boundLineRect[j].y > boundRect[i].y and boundLineRect[j].y < boundRect[i].br().y ) {
				float d1 = (float)(-boundLineRect[j].y + boundRect[i].br().y)/boundLineRect[j].height;
				float d2 = (float)(-boundLineRect[j].y + boundRect[i].br().y)/boundRect[i].height;
				float area = (float)boundRect[i].area()/boundLineRect[j].area();
				if ( d1>0.5 or d2>0.5 ) {
					if( boundLineRect[j].x > boundRect[i].x ){
						boundLineRect[j].width = boundLineRect[j].br().x - boundRect[i].x;
						boundLineRect[j].x = boundRect[i].x;
					} else if( boundLineRect[j].x + boundLineRect[j].width < boundRect[i].x + boundRect[i].width ) {
						boundLineRect[j].width = boundRect[i].x + boundRect[i].width - boundLineRect[j].x;

					}
					boundLineRect[j].height = -boundRect[i].y + boundLineRect[j].br().y;
					boundLineRect[j].y = boundRect[i].y;
					fit = true;
					fitCount.push_back(j);
				} else if ( area<0.008 ) {
					//printf("area=%f\n",area);
					if( boundLineRect[j].x > boundRect[i].x ){
						boundLineRect[j].width = boundLineRect[j].br().x - boundRect[i].x;
						boundLineRect[j].x = boundRect[i].x;
					} else if( boundLineRect[j].x + boundLineRect[j].width < boundRect[i].x + boundRect[i].width ) {
						boundLineRect[j].width = boundRect[i].x + boundRect[i].width - boundLineRect[j].x;

					}
					boundLineRect[j].height = -boundRect[i].y + boundLineRect[j].br().y;
					boundLineRect[j].y = boundRect[i].y;
					fit = true;
					fitCount.push_back(j);
				}
			}
			//rectangle( processed, boundLineRect[j].tl(), boundLineRect[j].br(), red, 1, 8, 0 );
		}
		if( fit == false )
			boundLineRect.push_back(boundRect[i]);
		if( fitCount.size()>1 ) {
			vector<Point> tempV;
			for(int k=0; k<fitCount.size(); k++)
			{
				tempV.push_back(boundLineRect[fitCount[k]].tl());
				tempV.push_back(boundLineRect[fitCount[k]].br());
			}
			boundLineRect[fitCount[0]] = boundingRect(tempV);
			for(int k=1; k<fitCount.size(); k++) 
				boundLineRect.erase(boundLineRect.begin() + fitCount[k]);
		}
		//imshow("Display window", processed);	
		//waitKey(0);
	}
	
	
        //imshow("Display window", grayImage);	
        //waitKey(0);
	
	// gg fd rg
	
	// dfgh h
	
	while(!boundLineRect.empty()){
		int max=0;
		unsigned int numberLines = boundLineRect.size();
		for(int i=1; i<numberLines; i++){
			if( boundLineRect[max].y > boundLineRect[i].y )
				max=i;
		}
		float area = (float)boundLineRect[max].area()/(binImage.rows*binImage.cols);
		//printf("area2=%f\n",area);
		if( area>0.001 )
			rectBoundStr.push_back(boundLineRect[max]);
		boundLineRect.erase(boundLineRect.begin() + max);
	}
	
	printf("Number of lines:%i\n", rectBoundStr.size());
	/*
	Scalar blu = Scalar( 237, 27, 27 );
	Scalar red = Scalar( 27, 27, 237 );
	for( int i = 0; i< rectBoundStr.size(); i++ )
	{
		//drawContours( src, contours_poly, i, color, 1, 8, vector<Vec4i>(), 0, Point() );
		rectangle( processed, rectBoundStr[i].tl(), rectBoundStr[i].br(), red, 1, 8, 0 );
	}
	
	namedWindow("Display window", WINDOW_AUTOSIZE);
        imshow("Display window", processed);	
        waitKey(0);
	//*/
	
	return 0;
}

int createName( std::string & name, const std::string & ext, const char* folder )
{
	int len, i=0;
	DIR *dir;
	struct dirent *ent;
	if ((dir = opendir (folder)) != NULL) {
		/* print all the files and directories within directory */
		while ((ent = readdir (dir)) != NULL) {
			const std::string fname = ent->d_name;
			len = fname.length();
			if (len >= ext.length()) {
				if ( !fname.compare(len-ext.length(), ext.length(), ext) ) {
					i++;
				}
			}
		}
		name = folder + SSTR(i) + ext;
		closedir (dir);
	} else {
		/* could not open directory */
		perror ("");
		return EXIT_FAILURE;
	}

	return 0;
}

int getScreen( Mat & sourceImage )
{
	int rx, ry, rw, rh;
	int rect_x = 0, rect_y = 0, rect_w = 0, rect_h = 0;
	bool btn_pressed = false, done = false;
	rw = 0;
	rh = 0;
	
	XEvent boxDrawingEvent;
	/// Conecting to the X server
	Display *defaultDisplay = XOpenDisplay(NULL);
	if(!defaultDisplay){
		perror ("");
		return EXIT_FAILURE;
	}
	
	Screen *scr = ScreenOfDisplay(defaultDisplay, DefaultScreen(defaultDisplay));
	
	Window root = RootWindow(defaultDisplay, XScreenNumberOfScreen(scr));
	
	Cursor arrow = XCreateFontCursor(defaultDisplay, XC_left_ptr);
	
	/// Most attributes of graphics operations are stored in Graphic Contexts (GCs)
	XGCValues gcval;
	gcval.foreground = XWhitePixel(defaultDisplay, 0);
	gcval.function = GXxor;
	gcval.background = XBlackPixel(defaultDisplay, 0);
	gcval.plane_mask = gcval.background ^ gcval.foreground;
	gcval.subwindow_mode = IncludeInferiors;
	
	GC gc;
	gc = XCreateGC(defaultDisplay, root,
                 GCFunction | GCForeground | GCBackground | GCSubwindowMode,
                 &gcval);
	
	if (XGrabPointer(
		defaultDisplay, root, false,
		ButtonMotionMask | ButtonPressMask | ButtonReleaseMask, GrabModeAsync,
		GrabModeAsync, root, arrow, CurrentTime)	!=	GrabSuccess) {
		printf("couldn't grab pointer\n");
		return EXIT_FAILURE;
	}

	if (XGrabKeyboard(
		defaultDisplay, root, false, GrabModeAsync, GrabModeAsync,
		CurrentTime)	!=	GrabSuccess) {
		printf("couldn't grab keyboard\n");
		return EXIT_FAILURE;
	}
	
	while (!done)
	{
		while (!done and XPending(defaultDisplay)>0 )
		{
			XNextEvent(defaultDisplay, &boxDrawingEvent);
			switch (boxDrawingEvent.type)
			{
				case ButtonPress:
				btn_pressed = true;
				rx = boxDrawingEvent.xbutton.x;
				ry = boxDrawingEvent.xbutton.y;
				break;
				
				case ButtonRelease:
				if( btn_pressed == true )
					done = true;
				break;
				
				case MotionNotify:
				/// This case is purely for drawing rect on screen
				if (btn_pressed)
				{
					/// Re-draw the last rect to clear it
					XDrawRectangle(defaultDisplay, root, gc, rect_x, rect_y, rect_w, rect_h);
					rect_x = rx;
					rect_y = ry;
					rect_w = boxDrawingEvent.xmotion.x - rect_x;
					rect_h = boxDrawingEvent.xmotion.y - rect_y;

					if (rect_w < 0) {
						rect_x += rect_w;
						rect_w = 0 - rect_w;
					}
					if (rect_h < 0) {
						rect_y += rect_h;
						rect_h = 0 - rect_h;
					}
					/// Draw rectangle
					XDrawRectangle(defaultDisplay, root, gc, rect_x, rect_y, rect_w, rect_h);
					XFlush(defaultDisplay);
				}
				break;
			}
		}
	}

	if(rect_w*rect_h==0){
		printf("Rectangle not selected\n");
		return EXIT_FAILURE;
	}
	/// Clear the drawn rectangle
	XDrawRectangle(defaultDisplay, root, gc, rect_x, rect_y, rect_w, rect_h);
	XFlush(defaultDisplay);
	
	rw = boxDrawingEvent.xbutton.x - rx;
	rh = boxDrawingEvent.xbutton.y - ry;
	/// Cursor moves backwards
	if (rw < 0) {
		rx += rw;
		rw = 0 - rw;
	}
	if (rh < 0) {
		ry += rh;
		rh = 0 - rh;
	}
	
	XImage *Ximg = XGetImage(defaultDisplay,root, rx,ry, rw,rh, AllPlanes, ZPixmap);
	int Bpp = Ximg->bits_per_pixel;
	sourceImage.create( rh, rw, Bpp > 24 ? CV_8UC4 : CV_8UC3 );
	memcpy(sourceImage.data, Ximg->data, rw*rh*4);

	XDestroyImage(Ximg);
	XCloseDisplay(defaultDisplay);
	
	return EXIT_SUCCESS;
}


#undef SSTR



