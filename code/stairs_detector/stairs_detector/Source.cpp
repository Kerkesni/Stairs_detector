#include <opencv2/core/mat.hpp>
#include <iostream>
#include <vector>
#include <sys/types.h>
#include <filesystem>

#include "decision.h"
#include "squares.h"
#include "hough.h"

namespace fs = std::filesystem;
using namespace cv;
using namespace std;


int main(int argc, char** argv)
{

	std::string path = "C:/Users/$USERNAME/Desktop/Stairs_detector/bdd/stairs";
	vector<vector<Point>> squares;

	// Loop for all images inside a directory
	for (const auto & entry : fs::directory_iterator(path)) {

		Mat image = imread(entry.path().string(), IMREAD_GRAYSCALE);
		if (image.empty())
		{
			cout << "Couldn't load " << entry.path() << endl;
			continue;
		}
		// getting the contours
		Mat contour = hought(image);
		// detecting the rectangles (or squares) objetcts in the image
		findSquares(contour, squares);

		// drawing the squares to the image
		Mat sq = drawSquares(image, squares, false);
		imshow("result", sq);
		
		// showing the resultng percentage
		cout << isStairs(squares)*100 << "%" << endl;

		int c = waitKey();
		if ((char)c == 27)
			break;
	}
}