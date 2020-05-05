#pragma once
#include <opencv2/core/core.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/imgproc/imgproc.hpp"
#include <vector>
#include <algorithm>
#include <set>

using namespace cv;
using namespace std;

void findSquares(const Mat& image, vector<vector<Point> >& squares);
Mat drawSquares(Mat& image, const vector<vector<Point> >& squares, bool isolate);