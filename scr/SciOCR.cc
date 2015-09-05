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
//~ #include <set>
#include "shredder.cc"


#include "multidim.h"
//~ #include "pymulti.h"
#include "extras.h"

//using namespace Eigen;
//using namespace ocropus;
//~ using namespace pymulti;
using multidim::mdarray;
using std::vector;
//~ using std::map;
//~ using std::make_pair;
//~ using std::shared_ptr;
//~ using std::unique_ptr;
using std::cout;
using std::cerr;
using std::endl;
//~ using std::ifstream;
//~ using std::set;
using std::to_string;
using std_string = std::string;
using std_wstring = std::wstring;
#define string std_string
#define wstring std_wstring

int ocr( const vector<Rect> & , const Mat & , ocropus::CLSTMOCR & );

int main1(int argc, char **argv)
{
	ocropus::CLSTMOCR clstm;
	clstm.load("./net/ruphy-28000.model.proto");
	
	Mat sourceImage;
	vector<Rect> rectBoundStr;
	sourceImage = imread("./img/3.bmp");
	
	shredder(rectBoundStr, sourceImage); 
	
	Mat tmpImage;
	cvtColor( sourceImage, tmpImage, CV_RGB2GRAY );
	
	ocr(rectBoundStr, tmpImage, clstm);
	
	return 0;
}

int main(int argc, char **argv) {
#ifdef NOEXCEPTION
  return main1(argc, argv);
#else
  try {
    return main1(argc, argv);
  } catch (const char *message) {
    cerr << "FATAL: " << message << endl;
  }
#endif
}

int ocr( const vector<Rect> & rectBoundStr, const Mat & Image, ocropus::CLSTMOCR & clstm )
{
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
			string out = clstm.predict_utf8(raw);
			cout << out << endl;
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
}


#undef string
#undef wstring