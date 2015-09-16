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
//~ #include <set>
//~ #include "shredder.cc"


#include "multidim.h"
//~ #include "pymulti.h"
#include "extras.h"
#include <boost/python.hpp>
#include "OCR.h"
#include "image_processing.h"
using namespace cv;




PyObject* getDataFromMat( Mat const& img )
{
	Mat tmp;
	cvtColor(img, tmp, CV_BGR2RGB);
	Size s = tmp.size();
	int w = s.width;
	int h = s.height;
	PyObject* py_buf = PyBuffer_FromMemory(tmp.data, w*h*3);
	return py_buf;
}

PyObject* textToByte( std::string src )
{
	return PyBytes_FromStringAndSize( src.c_str(), src.size() );
}

void loadNet(ocropus::CLSTMOCR & clstm, char* path ) {
	if( fileExists(path))
		clstm.load(path);
}

void loadImg(Mat & sourceImage, char* path ) {
	if( fileExists(path))
		sourceImage = imread(path, CV_LOAD_IMAGE_COLOR);
}

int saveImg( Mat const& img ) {
	std::string name; 
	/// Generate name like: 1.bmp, 2.bmp ... if it is not already exist 
	if( createName(name, ".bmp", "./data/book/img/")==EXIT_FAILURE )
		return 0;
	/// And save it to ./data/img/
	imwrite( name, img );
	return 0;
}

int saveText( std::string const& text ) {
	std::string name; 
	/// Generate name like: 1.txt, 2.txt ... if it is not already exist 
	if( createName(name, ".txt", "./data/book/txt/")==EXIT_FAILURE )
		return 0;
	/// And save it to ./data/txt/
	std::ofstream fout;
	fout.open(name.c_str());
	fout << text;
	fout.close();
	return 0;
}

void SaveAll( Mat const& sourceImage, Mat const& processed, vector<Rect> const& boundLineRect, std::string const& text ) {
	std::string textName;
	createName(textName, ".txt", "./data/book/txt/");
	std::string imgName;
	createName(imgName, ".bmp", "./data/book/img/");
	
	imwrite( imgName, sourceImage );
	std::ofstream fout;
	fout.open(textName.c_str());
	fout << text;
	fout.close();
	
	saveShred( boundLineRect, processed, textName.c_str() );
}


BOOST_PYTHON_MODULE(SciOCR)
{
	using namespace boost::python;
	def("ocr", ocr);
	def("preprocessing", preprocessing);
	def("shredder", shredder);
	def("getScreen", getScreen);
	def("saveText", saveText);
	def("SaveAll", SaveAll);
	class_<Mat>("Mat")
		.def_readonly("cols", &Mat::cols )
		.def_readonly("rows", &Mat::rows )
		.def("data", getDataFromMat)
		.def("load", loadImg)
		.def("save", saveImg)
	;
	
	class_<ocropus::CLSTMOCR, boost::noncopyable>("Clstm")
		//~ .def("__init__", loadNet )
		.def("load", loadNet )
	;
	
	class_<Rect>("Rect");
	class_<std::vector<Rect> >("vect_of_rect");
	//~ class_<std::vector<std::string> >("vect_of_str");
	
	//~ to_python_converter< Mat, type_into_python<Mat> >();
	//~ to_python_converter< ptime, type_into_python<ptime> >();
}


