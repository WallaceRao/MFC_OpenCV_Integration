#pragma once
#include <cv.h>
using namespace cv;
class MatCImageConverter
{
public:
	MatCImageConverter();
	~MatCImageConverter();
	// Convert OpenCV Mat to MFC CImage 
	void MatToCImage(Mat& mat, CImage& cimage);
	// Convert MFC CImage to OpenCV Mat 
	void CImageToMat(CImage& cimage, Mat& mat);
};

