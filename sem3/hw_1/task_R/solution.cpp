#include <iostream>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <iterator>
#include <cmath>
#include <cstdlib>

using namespace std;


struct Point2D {
	int x, y;
};

istream& operator >> (istream &is, Point2D &point) {
	return is >> point.x >> point.y;
}

ostream& operator << (ostream &os, const Point2D &point) {
	return os << point.x << " " << point.y;
}

bool operator == (const Point2D &firstPoint, const Point2D &secondPoint) {
	return firstPoint.x == secondPoint.x && firstPoint.y == secondPoint.y;
}

long double distance (const Point2D &firstPoint, const Point2D &secondPoint) {
	return sqrt(pow((firstPoint.x - secondPoint.x), 2) + pow((firstPoint.y - secondPoint.y), 2));
}



template<class RandomAccessIterator>
long double findMinDistanceRecursion (const RandomAccessIterator &begin, const RandomAccessIterator &end) {
	unsigned halfIndex = (distance(begin, end) - 1) / 2;
	RandomAccessIterator middle = next(begin, halfIndex);

	if (distance(begin, end) == 2)
		return distance(*begin, *prev(end));

	else if (distance(begin, end) == 3)
		return min<long double>(min<long double>(distance(*begin, *middle), distance(*middle, *prev(end))), distance(*begin, *prev(end)));


	long double minimumDistanceInFirstSubset = findMinDistanceRecursion(begin, next(middle));
	long double minimumDistanceInSecondSubset = findMinDistanceRecursion(next(middle), end);
	long double minimumDistanceInSubsets = min<long double>(minimumDistanceInFirstSubset, minimumDistanceInSecondSubset);

	vector<Point2D> firstProbe;

	for (RandomAccessIterator i = begin; i != end; i ++)
		if (abs(i->x - middle->x) < minimumDistanceInSubsets)
			firstProbe.push_back(*i);


	sort(firstProbe.begin(), firstProbe.end(), [](const Point2D &firstPoint, const Point2D &secondPoint) {
		return firstPoint.y < secondPoint.y;
	});



	for (unsigned i = 1; i < firstProbe.size(); i ++)
		for (int j = i - 1; firstProbe[i].y - minimumDistanceInSubsets < firstProbe[j].y && j >= 0; j --)
			if (distance(firstProbe[i], firstProbe[j]) < minimumDistanceInSubsets)
				minimumDistanceInSubsets = distance(firstProbe[i], firstProbe[j]);

	return minimumDistanceInSubsets;
}

long double findMinDistance (const vector<Point2D> &points) {
	vector<Point2D> sortedPoints = points;

	sort(sortedPoints.begin(), sortedPoints.end(), [](const Point2D &firstPoint, const Point2D &secondPoint) {
		return firstPoint.x < secondPoint.x || (firstPoint.x == secondPoint.x && firstPoint.y < secondPoint.y);
	});


	for (unsigned i = 1; i < sortedPoints.size(); i ++)
		if (sortedPoints[i - 1] == sortedPoints[i])
			return 0;

	return findMinDistanceRecursion(sortedPoints.begin(), sortedPoints.end());
}



int main () {
	unsigned pointsCount;
	cin >> pointsCount;

	vector<Point2D> points(pointsCount);

	for (unsigned i = 0; i < pointsCount; i ++)
		cin >> points[i];

	cout << fixed << setprecision(10) << findMinDistance(points) << endl;

	return 0;
}