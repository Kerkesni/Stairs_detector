#include "opencv2/imgproc/imgproc.hpp"
#include "edge.h"

using namespace cv;
using namespace std;

int lowThreshold = 50;
const int rt = 3;
const int kernel_size = 3;

Mat gammaCorrection(const Mat &img, const double gamma_)
{
	CV_Assert(gamma_ >= 0);
	//! [changing-contrast-brightness-gamma-correction]
	Mat lookUpTable(1, 256, CV_8U);
	uchar* p = lookUpTable.ptr();
	for (int i = 0; i < 256; ++i)
		p[i] = saturate_cast<uchar>(pow(i / 255.0, gamma_) * 255.0);

	Mat res = img.clone();
	LUT(img, lookUpTable, res);
	//! [changing-contrast-brightness-gamma-correction]
	return res;
}


//edge detection
Mat getEdges(Mat src) {
	Mat src_gray, detected_edges, dst;

	//gama level correction for dark images
	gammaCorrection(src, 0.5);

	// converting image to grayscale
	cvtColor(src, src_gray, COLOR_BGR2GRAY);

	// applying gaussian blur to reduce noise
	blur(src_gray, detected_edges, Size(3, 3));

	// appying canny edge detection
	Canny(detected_edges, detected_edges, lowThreshold, lowThreshold*rt, kernel_size);

	// blacking out the background
	dst = Scalar::all(0);
	src.copyTo(dst, detected_edges);

	return detected_edges;
}
