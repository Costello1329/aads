#include <iostream>
#include <vector>
#include <fstream>
#include <algorithm>
#include <cmath>
#include <iomanip>

using namespace std;
typedef long double ld;
ld epsilon = 0.000000001f;


template <typename Type>
vector<ld> solveQuadraticEquasion (const Type &a, const Type &b, const Type &c) {
	vector<ld> roots;

	if (a == 0) {
		if (b == 0)
			roots = vector<ld>(3, 0);

		else
			roots.push_back(- c/b);
	}

	else {
		ld D2 = b*b - 4*a*c;

		if (D2 > 0) {
			roots.push_back((-b + sqrt(D2)) / (2*a));
			roots.push_back((-b - sqrt(D2)) / (2*a));
		}

		else if (D2 == 0)
			roots.push_back(- b/(2*a));
	}

	return roots;
}


template <typename Type>
struct Vector2D {
	Type x, y;

	Vector2D (const Type &x = 0, const Type &y = 0) {
		this->x = x;
		this->y = y;
	}

	ld length () const {
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
Type dotProduct (const Vector2D<Type> &firstVector, const Vector2D<Type> &secondVector) {
	return firstVector.x*secondVector.x + firstVector.y*secondVector.y;
}

template <typename Type>
Type crossProduct (const Vector2D<Type> &firstVector, const Vector2D<Type> &secondVector) {
	return firstVector.x*secondVector.y - firstVector.y*secondVector.x;
}


template <typename Type>
struct Point2D {
	Type x, y;

	Point2D (const Type &x = 0, const Type &y = 0) {
		this->x = x;
		this->y = y;
	}
};

template <typename Type>
bool operator == (const Point2D<Type> &firstPoint, const Point2D<Type> &secondPoint) {
	return (firstPoint.x == secondPoint.x && firstPoint.y == secondPoint.y);
}

template <typename Type>
bool operator != (const Point2D<Type> &firstPoint, const Point2D<Type> &secondPoint) {
	return !(firstPoint == secondPoint);
}

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
ostream& operator << (ostream &os, const Point2D<Type> &point) {
	return os << point.x << " " << point.y;
}

template <typename Type>
istream& operator >> (istream &is, Point2D<Type> &point) {
	return is >> point.x >> point.y;
}


template <typename Type>
struct Segment2D {
	Point2D<Type> start, finish;

	Segment2D (const Point2D<Type> &start = Point2D<Type>(), const Point2D<Type> &finish = Point2D<Type>()) {
		this->start = start;
		this->finish = finish;
	}
};

template <typename Type>
istream& operator >> (istream &is, Segment2D<Type> &segment) {
	return is >> segment.start >> segment.finish;
}


template <typename Type>
struct Box2D {
	Point2D<Type> leftCorner, rightCorner;

	Box2D (const Point2D<Type> &leftCorner = Point2D<Type>(), const Point2D<Type> &rightCorner = Point2D<Type>()) {
		this->leftCorner = leftCorner;
		this->rightCorner = rightCorner;
	}
};


template <typename Type>
bool isPointInBox (const Point2D<Type> &point, const Box2D<Type> &box) {
	return (box.leftCorner.x <= point.x && point.x <= box.rightCorner.x) && (box.leftCorner.y <= point.y && point.y <= box.rightCorner.y);
}

template <typename Type>
bool isPointOnSegment (const Point2D<Type> &point, const Segment2D<Type> &segment) {
	Vector2D<Type> firstVector = segment.finish - segment.start;
	Vector2D<Type> secondVector = point - segment.start;
	Point2D<Type> leftCorner(min<Type>(segment.start.x, segment.finish.x), min<Type>(segment.start.y, segment.finish.y));
	Point2D<Type> rightCorner(max<Type>(segment.start.x, segment.finish.x), max<Type>(segment.start.y, segment.finish.y));
	Box2D<Type> box = Box2D<Type>(leftCorner, rightCorner);
	return abs(crossProduct(firstVector, secondVector)) <= epsilon && isPointInBox(point, box);
}

template <typename Type>
vector<Point2D<Type> > getLineAndCentralCircleIntersection (const Type &radius, const Vector2D<Type> &lineStart, const Vector2D<Type> &lineDirection) {
	Type a = dotProduct(lineDirection, lineDirection);
	Type b = 2 * dotProduct(lineStart, lineDirection);
	Type c = dotProduct(lineStart, lineStart) - radius*radius;
	vector<ld> roots = solveQuadraticEquasion(a, b, c);

	vector<Point2D<Type> > intersections;

	for (const ld &root: roots) {
		Point2D<Type> point = Point2D<Type>(0, 0) + lineStart + (Type)root * lineDirection;		
		intersections.push_back(point);
	}

	return intersections;
}


template <typename Type>
vector<Point2D<Type> > getSegmentAndCircleIntersection (const Segment2D<Type> &segment, const Point2D<Type> &center, const Type &radius) {
	vector<Point2D<ld> > intersections;

	if (segment.start == segment.finish && radius == 0) {
		if (segment.start == center)
			intersections.push_back(segment.start);
	}
	
	else if (segment.start == segment.finish && radius != 0) {
		Vector2D<ld> vec = segment.start - center;
	
		if (vec.length() == radius)
			intersections.push_back(segment.start);
	}
	
	else if (segment.start != segment.finish && radius == 0) {
		if (isPointOnSegment(center, segment))
			intersections.push_back(center);
	}
	
	else {
		Vector2D<ld> lineStart = segment.start - center;
		Vector2D<ld> lineDirection = segment.finish - segment.start;
		vector<Point2D<ld> > lineWithCircleIntersections = getLineAndCentralCircleIntersection(radius, lineStart, lineDirection);
	
		for (const Point2D<ld> &rawPoint: lineWithCircleIntersections) {
			Point2D<ld> point = rawPoint + center;
	
			if (isPointOnSegment(point, segment))
				intersections.push_back(point);
		}
	}
	
	sort(intersections.begin(), intersections.end(),  [](const Point2D<ld> &firstPoint, const Point2D<ld> &secondPoint) {  
		return (firstPoint.x < secondPoint.x || (firstPoint.x == secondPoint.x && firstPoint.y < secondPoint.y));
	});

	return intersections;
}



int main () {
	while (true) {
		Point2D<ld> center;
		ld radius;
		Segment2D<ld> segment;

		if(!(cin >> center >> radius >> segment))
			break;
	
		vector<Point2D<ld> > intersections = getSegmentAndCircleIntersection(segment, center, radius);

		cout << intersections.size() << endl;
	
		for (const Point2D<ld> &intersection: intersections)
			cout << fixed << setprecision(10) << intersection << endl;

		cout << endl;
	}

	return 0;
}
