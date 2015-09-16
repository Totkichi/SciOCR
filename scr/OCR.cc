#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "pstring.h"
#include "clstm.h"
#include "clstmhl.h"
#include <assert.h>
#include <iostream>
#include <vector>
#include <memory>
#include <math.h>
#include <Eigen/Dense>
#include <sstream>
#include <fstream>
#include "image_processing.h"

#include "multidim.h"
#include "extras.h"
#include "OCR.h"

using multidim::mdarray;
using std::vector;
using std::unique_ptr;
using std::cout;
using std::cerr;
using std::endl;
using std::to_string;
using std::string;
using std::wstring;
using cv::Mat;
using cv::Rect;


string ocr( const vector<Rect> & rectBoundStr, const Mat & Image, ocropus::CLSTMOCR & clstm )
{
	string out = "";
	bool conf = ocropus::getienv("conf", 0);
	for(int j=0; j<rectBoundStr.size(); j++)
	{
		mdarray<float> raw;
		Mat shred;
		shred = Image(rectBoundStr[j]);
		raw.resize(shred.cols, shred.rows);
		for(int h = 0; h < shred.rows; h++) {
			for(int w=0; w<shred.cols; w++)
				raw(w,h) = 1. - (float)shred.at<uchar>(h, w)/255.;
		}
		if (!conf) {
			string line = clstm.predict_utf8(raw);
			out += line;
			out += "\n";
		} else {
			vector<ocropus::CharPrediction> preds;
			clstm.predict(preds, raw);
			for (int i = 0; i < preds.size(); i++)
			{
				ocropus::CharPrediction p = preds[i];
				const char *sep = "\t";
				cout << p.i << sep << p.x << sep << p.c << sep << p.p << endl;
			}
		}
	}
	
	return out;
}

