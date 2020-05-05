#pragma once
#include <opencv2/core/core.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/imgproc/imgproc.hpp"
#include <vector>

using namespace cv;
using namespace std;


bool are_paralel(vector<Point> p0, vector<Point> p1);
float isStairs(const vector<vector<Point> >& squares);