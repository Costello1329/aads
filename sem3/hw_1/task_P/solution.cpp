#include <iostream>
#include <vector>
#include <fstream>
#include <algorithm>
#include <iomanip>
#include <set>
#include <cmath>
#include <iterator>

const long double pi = M_PI;

using namespace std;


// VECTOR:

template <typename Type>
struct Vector2D {
	Type x, y;

	Vector2D (const Type &x = 0, const Type &y = 0) {
		this->x = x;
		this->y = y;
	}

	long double length () const {
		return sqrt(x*x + y*y);
	}
};

template <typename Type>
ostream& operator << (ostream &os, const Vector2D<Type> &vector) {
	return os << vector.x << " " << vector.y;
}

template <typename Type>
istream& operator >> (istream &is, const Vector2D<Type> &vector) {
	return is >> vector.x >> vector.y;
}

template <typename Type>
Vector2D<Type> operator - (const Vector2D<Type> &firstVector, const Vector2D<Type> &secondVector) {
	return Vector2D<Type>(firstVector.x - secondVector.x, firstVector.y - secondVector.y);
}

template <typename Type>
Vector2D<Type> operator + (const Vector2D<Type> &firstVector, const Vector2D<Type> &secondVector) {
	return Vector2D<Type>(firstVector.x + secondVector.x, firstVector.y + secondVector.y);
}

template <typename Type>
Vector2D<Type> operator * (const Type &number, const Vector2D<Type> &vector) {
	return Vector2D<Type>(number * vector.x, number * vector.y);
}

template <typename Type>
Vector2D<Type> operator * (const Vector2D<Type> &vector, const Type &number) {
	return Vector2D<Type>(number * vector.x, number * vector.y);
}

template <typename Type>
Vector2D<Type> operator / (const Vector2D<Type> &vector, const Type &number) {
	return Vector2D<Type>(vector.x / number, vector.y / number);
}

template <typename Type>
Type dotProduct (const Vector2D<Type> &firstVector, const Vector2D<Type> &secondVector) {
	return firstVector.x*secondVector.x + firstVector.y*secondVector.y;
}

template <typename Type>
Type crossProduct (const Vector2D<Type> &firstVector, const Vector2D<Type> &secondVector) {
	return firstVector.x*secondVector.y - firstVector.y*secondVector.x;
}

template <typename Type>
long double countAngleBetweenVectors (const Vector2D<Type> &firstVector, const Vector2D<Type> &secondVector) {
	return atan2(crossProduct(firstVector, secondVector), dotProduct(firstVector, secondVector));
}


//POINT:

template <typename Type>
struct Point2D {
	Type x, y;

	Point2D (const Type &x = 0, const Type &y = 0) {
		this->x = x;
		this->y = y;
	}
};

template <typename Type>
Vector2D<Type> operator - (const Point2D<Type> &firstPoint, const Point2D<Type> &secondPoint) {
	return Vector2D<Type>(firstPoint.x - secondPoint.x, firstPoint.y - secondPoint.y);
}

template <typename Type>
Point2D<Type> operator + (const Point2D<Type> &firstPoint, const Point2D<Type> &secondPoint) {
	return Point2D<Type>(firstPoint.x + secondPoint.x, firstPoint.y + secondPoint.y);
}

template <typename Type>
Point2D<Type> operator + (const Point2D<Type> &point, const Vector2D<Type> &vector) {
	return Point2D<Type>(point.x + vector.x, point.y + vector.y);
}

template <typename Type>
bool operator == (const Point2D<Type> &firstPoint, const Point2D<Type> &secondPoint) {
	return (firstPoint.x == secondPoint.x && firstPoint.y == secondPoint.y);
}

template <typename Type>
bool operator != (const Point2D<Type> &firstPoint, const Point2D<Type> &secondPoint) {
	return !(firstPoint == secondPoint);
}

template <typename Type>
bool operator > (const Point2D<Type> &firstPoint, const Point2D<Type> &secondPoint) {
	return firstPoint.x > secondPoint.x || (firstPoint.x == secondPoint.x && firstPoint.y > secondPoint.y);
}

template <typename Type>
bool operator < (const Point2D<Type> &firstPoint, const Point2D<Type> &secondPoint) {
	return !(firstPoint > secondPoint) && firstPoint != secondPoint;
}

template <typename Type>
bool operator >= (const Point2D<Type> &firstPoint, const Point2D<Type> &secondPoint) {
	return firstPoint > secondPoint || firstPoint == secondPoint;
}

template <typename Type>
bool operator <= (const Point2D<Type> &firstPoint, const Point2D<Type> &secondPoint) {
	return firstPoint < secondPoint || firstPoint == secondPoint;
}

template <typename Type>
ostream& operator << (ostream &os, const Point2D<Type> &point) {
	return os << point.x << " " << point.y;
}

template <typename Type>
istream& operator >> (istream &is, Point2D<Type> &point) {
	return is >> point.x >> point.y;
}


// CONVEX HULL:

template <typename Type>
long double rotate (const Point2D<Type> &point1, const Point2D<Type> &point2, const Point2D<Type> &point3) {
	return crossProduct(point2 - point1, point3 - point2);
}

template <typename Type>
long double length (const Vector2D<Type> &vector) {
	return sqrt(dotProduct(vector, vector));
}

template <typename Type>
vector<Point2D<Type> > buildConvexHullGraham (const vector<Point2D<Type> > &points) {
	if (points.size() <= 2)
		return points;

	Point2D<Type> firstPoint = points[0];

	for (const Point2D<Type> &point: points)
		if (point.x < firstPoint.x || (point.x == firstPoint.x && point.y < firstPoint.y))
			firstPoint = point;


	vector<Point2D<Type> > sortedPoints = points;

	sort(sortedPoints.begin(), sortedPoints.end(), [firstPoint] (const Point2D<Type> &point1, const Point2D<Type> &point2) {
		long double predicate = rotate(point2, firstPoint, point1);

		if (predicate == 0)
			return length(point1 - firstPoint) < length(point2 - firstPoint);
	
		else
			return predicate > 0;
	});


	vector<Point2D<Type> > convexHull;
	convexHull.push_back(sortedPoints[0]);
	convexHull.push_back(sortedPoints[1]);

	for (unsigned i = 2; i < sortedPoints.size(); i ++) {
		while (convexHull.size() >= 2 && rotate(convexHull[convexHull.size() - 2], convexHull[convexHull.size() - 1], sortedPoints[i]) <= 0)
			convexHull.pop_back();

		convexHull.push_back(sortedPoints[i]);
	}

	return convexHull;
}


// MAIN:

template <typename Type>
long double countPerimeterOfWall (const Type &radius, const vector<Point2D<Type> > &points) {
	vector<Point2D<Type> > convexHull = buildConvexHullGraham(points);
	
	/*cout << "convex hull (" << convexHull.size() << ") :" << endl;
	for (const Point2D<Type> &point: convexHull)
		cout << point << endl;*/

	vector<Point2D<Type> > newConvexHull;

	for (unsigned i = 0; i < convexHull.size(); i ++) {
		Point2D<Type> point1 = convexHull[i];
		Point2D<Type> point2 = convexHull[(i + 1) % convexHull.size()];
		Vector2D<Type> vector = point2 - point1;
		Vector2D<Type> normal = Vector2D<Type>(vector.y, -vector.x);
		Vector2D<Type> radiusVector = normal / length(normal) * radius;
		newConvexHull.push_back(point1 + radiusVector);
		newConvexHull.push_back(point2 + radiusVector);
	}


	/*cout << "new convex hull (" << newConvexHull.size() << ") :" << endl;
	for (const Point2D<Type> &point: newConvexHull)
		cout << point << endl;*/

	long double perimeter = 0;

	for (unsigned i = 0; i < newConvexHull.size(); i ++) {
		Point2D<Type> point1 = newConvexHull[i];
		Point2D<Type> point2 = newConvexHull[(i + 1) % newConvexHull.size()];
		
		if (i % 2 == 0) {
			perimeter += length(point2 - point1);
		}

		else {
			Point2D<Type> center = convexHull[((i + 1) / 2) % convexHull.size()];
			Vector2D<Type> radiusVector1 = point1 - center;
			Vector2D<Type> radiusVector2 = point2 - center;
			long double angle = countAngleBetweenVectors(radiusVector1, radiusVector2);
			perimeter += abs(angle) * radius;
		}
	}

	return perimeter;
}


int main () {
	unsigned pointsNumber;
	cin >> pointsNumber;

	long double radius;
	cin >> radius;

	vector<Point2D<long double> > points(pointsNumber);

	for (unsigned i = 0; i < pointsNumber; i ++)
		cin >> points[i].x >> points[i].y;
	
	cout << fixed << setprecision(10) << countPerimeterOfWall(radius, points) << endl;

	return 0;
}
