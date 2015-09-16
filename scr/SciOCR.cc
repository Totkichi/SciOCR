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
#include "OCR.cc"

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
