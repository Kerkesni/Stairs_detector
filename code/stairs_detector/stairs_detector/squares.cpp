#include "squares.h"

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

// finds a cosine of angle between vectors
// from pt0->pt1 and from pt0->pt2
double angle(Point pt1, Point pt2, Point pt0)
{
	double dx1 = pt1.x - pt0.x;
	double dy1 = pt1.y - pt0.y;
	double dx2 = pt2.x - pt0.x;
	double dy2 = pt2.y - pt0.y;
	return (dx1*dx2 + dy1 * dy2) / sqrt((dx1*dx1 + dy1 * dy1)*(dx2*dx2 + dy2 * dy2) + 1e-10);
}

// calculates the area inside a set of 4 point
float getArea(vector<Point> vp) {
	float sub1 = (vp[0].x*vp[1].y - vp[1].x*vp[0].y);
	float sub2 = (vp[1].x*vp[2].y - vp[2].x*vp[1].y);
	float sub3 = (vp[2].x*vp[3].y - vp[3].x*vp[2].y);
	float sub4 = (vp[3].x*vp[0].y - vp[0].x*vp[3].y);

	return abs((sub1 + sub2 + sub3 + sub4) / 2);
}

// returns sequence of squares detected on the image.
// the sequence is stored in the specified memory storage
void findSquares(const Mat& image, vector<vector<Point> >& squares)
{
	int thresh = 15;
	int N = 2;
	// accespted angle between rectangle vertecies
	float cosAngle = 0.8;
	// rectange area related variables
	float accumulator_areas = 0;
	float acc_index = 0;
	float Lcoef = 0.5;
	float Hcoef = 2;
	// temporary variables
	vector <vector<Point>> tmp_squares;

	squares.clear();
	
	// Correct gamma level
	//gammaCorrection(image, 0.1);

	// blur to enhance edge detection
	Mat timg = image.clone();
	medianBlur(image, timg, 9);
	Mat gray(timg.size(), CV_8U);
	vector<vector<Point> > contours;


	int ch[] = { 0, 0 };
	mixChannels(&image, 1, &gray, 1, ch, 1);

	// Apply N times to maximise the chances of finding a rectangle
	for (int j = 0; j < N; j++) {
		// apply Canny. Take the upper threshold from slider
		// and set the lower to 0 (which forces edges merging)
		Canny(gray, gray, 5, thresh, 5);
		// dilate canny output to remove potential
		// holes between edge segments
		dilate(gray, gray, Mat(), Point(-1, -1));

		// find contours and store them all as a list
		findContours(gray, contours, RETR_LIST, CHAIN_APPROX_SIMPLE);

		vector<Point> approx;

		// test each contour
		for (size_t i = 0; i < contours.size(); i++)
		{
			// approximate contour with accuracy proportional
			// to the contour perimeter
			approxPolyDP(Mat(contours[i]), approx, arcLength(Mat(contours[i]), true)*0.02, true);

			// square contours should have 4 vertices after approximation
			// relatively large area (to filter out noisy contours)
			// and be convex.
			// Note: absolute value of an area is used because
			// area may be positive or negative - in accordance with the
			// contour orientation
			if (approx.size() == 4 &&
				fabs(contourArea(Mat(approx))) > 1000 &&
				isContourConvex(Mat(approx)))
			{
				double maxCosine = 0;

				for (int j = 2; j < 5; j++)
				{
					// find the maximum cosine of the angle between joint edges
					double cosine = fabs(angle(approx[j % 4], approx[j - 2], approx[j - 1]));
					maxCosine = MAX(maxCosine, cosine);
				}

				// if cosines of all angles are small
				// (all angles are ~90 degree) then write quandrange
				// vertices to resultant sequence
				if (maxCosine < cosAngle) {
					tmp_squares.push_back(approx);
					accumulator_areas += getArea(approx);
					acc_index++;
				}
			}
		}
		//storing all the rectangles found
		squares.insert(squares.begin(), tmp_squares.begin(), tmp_squares.end());

	}
	//we only keep the rectangles in a certain range of size (around the mean of all sizes)
	squares.erase(
		std::remove_if(squares.begin(), squares.end(),
			[accumulator_areas, acc_index, Lcoef, Hcoef](const vector<Point> & vp) {
		float mean = (float)(accumulator_areas / acc_index);
		bool tooLow = getArea(vp) < mean*Lcoef;
		bool tooHigh = getArea(vp) > mean+mean*Hcoef;
		return tooLow || tooHigh;
			}),
		squares.end());
}

// the function draws all the squares in the image
Mat drawSquares(Mat& image, const vector<vector<Point> >& squares, bool isolate)
{
	Mat tmp;
	Scalar color;

	//if isolate we only show the rectangles in a blank image
	// else we show them inside the original image
	if (isolate) {
		tmp = Mat1f(image.size());
		color = Scalar(255, 255, 255);
	}
	else {
		tmp = image.clone();
		color = Scalar(255, 0, 0);
	}

	int nb = squares.size();
	int res = 0;

	if (squares.size() == 0)
		return tmp;

	// drawing the rectangles to the image
	for (size_t i = 0; i < squares.size() - 1; i++)
	{
		const Point* p = &squares[i][0];

		int n = (int)squares[i].size();

		if (p->x > 3 && p->y > 3)
			polylines(tmp, &p, &n, 1, true, color, 3, LINE_AA);
	}
	return tmp;
}
