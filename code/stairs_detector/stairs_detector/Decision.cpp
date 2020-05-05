#include "decision.h"
#include <iostream>

// calclates the deteminant of two edges and returns true if they are parallel (or almost parallel, a small degree of error is accepted)
bool are_paralel(vector<Point> p0, vector<Point> p1) {
	float AX = p0[1].x - p0[0].x;
	float AY = p0[1].y - p0[0].y;
	float BX = p1[1].x - p1[0].x;
	float BY = p1[1].y - p1[0].y;
	float det = AX * BY - AY * BX;
	return (0.5 < det < 0.5);
}

//Given three colinear points p, q, r, the function checks if
// point q lies on line segment 'pr' 
bool onSegment(Point p, Point q, Point r)
{
	if (q.x <= max(p.x, r.x) && q.x >= min(p.x, r.x) &&
		q.y <= max(p.y, r.y) && q.y >= min(p.y, r.y))
		return true;

	return false;
}

// To find orientation of ordered triplet (p, q, r). 
// The function returns following values 
// 0 --> p, q and r are colinear 
// 1 --> Clockwise 
// 2 --> Counterclockwise 
int orientation(Point p, Point q, Point r)
{
	int val = (q.y - p.y) * (r.x - q.x) -
		(q.x - p.x) * (r.y - q.y);

	if (val == 0) return 0;  // colinear 

	return (val > 0) ? 1 : 2; // clock or counterclock wise 
}

// returns true if two vectors intersect
bool doIntersect(Point p1, Point q1, Point p2, Point q2)
{
	// Find the four orientations needed for general and 
	// special cases 
	int o1 = orientation(p1, q1, p2);
	int o2 = orientation(p1, q1, q2);
	int o3 = orientation(p2, q2, p1);
	int o4 = orientation(p2, q2, q1);

	// General case 
	if (o1 != o2 && o3 != o4)
		return true;

	// Special Cases 
	// p1, q1 and p2 are colinear and p2 lies on segment p1q1 
	if (o1 == 0 && onSegment(p1, p2, q1)) return true;

	// p1, q1 and q2 are colinear and q2 lies on segment p1q1 
	if (o2 == 0 && onSegment(p1, q2, q1)) return true;

	// p2, q2 and p1 are colinear and p1 lies on segment p2q2 
	if (o3 == 0 && onSegment(p2, p1, q2)) return true;

	// p2, q2 and q1 are colinear and q1 lies on segment p2q2 
	if (o4 == 0 && onSegment(p2, q1, q2)) return true;

	return false; // Doesn't fall in any of the above cases 
}

//returns true if one of the vectors is directly on top of the other
bool isOnTop(vector<Point> p0, vector<Point> p1) {
	// vertical vector coords
	Point bottom_point;
	Point top_point;

	// top most vector
	vector<Point> top_line;

	// setting up the coords of the vectors
	if ((p0[0].y + p0[1].y) / 2 < (p1[0].y + p1[1].y) / 2) {
		bottom_point.y = (p0[0].y + p0[1].y) / 2;
		bottom_point.x = (p0[0].x + p0[1].x) / 2;

		top_point.y = std::max(p1[0].y, p1[1].y);
		top_point.x = bottom_point.x;

		top_line = p1;
	}
	else {
		bottom_point.y = (p1[0].y + p1[1].y) / 2;
		bottom_point.x = (p1[0].x + p1[1].x) / 2;

		top_point.y = std::max(p0[0].y, p0[1].y);
		top_point.x = bottom_point.x;

		top_line = p0;
	}

	return doIntersect(bottom_point, top_point, top_line[0], top_line[1]);

}

// returns a value between 0 and 1 if stairs detected (probability of it beeing stairs)
// or -0.01 if the number of squares detected is not enought to decide
float isStairs(const vector<vector<Point> >& squares) {

	float coef = 0.9;

	int total = 0;
	float res = 0;

	// if no enough squares we return -0.01 (-1%)
	if (squares.size() <= 2)
		return -0.01;

	//else we loop over all the detected squares
	// and compare their top most edges with all the top edges of the other squares
	for (size_t i = 0; i < squares.size() - 1; i++)
	{
		int local = 0;
		int local_total = 0;
		for (size_t j = i + 1; j < squares.size(); j++)
		{
			//we consider the compare all the edges of the two squares and take the best case
			//best case == the two edges are parallel and are on top of each other
			int top = 0;
			for (int k = 0; k < 4; k++) {
			vector<Point> v1{ squares[i][k], squares[i][(k+3)%4] };
				for (int l = 0; l < 4; l++) {
					vector<Point> v2{ squares[j][l], squares[j][(l+3)%4] };
					if (are_paralel(v1, v2) && isOnTop(v1, v2))
						top == 0 ? top++: top;
				}
			}
			local += top;
			local_total++;
			total++;
		}
		//we consider a rectangle a stair if it validates the two rules cited above (parallel + on top) with at least coef% of the total number of squares
		if (local > local_total*coef)
			res += local_total;
		else
			res += local;
	}
	return (float)res / (float)total;
}