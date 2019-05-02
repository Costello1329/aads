#include <iostream>
#include <iomanip>
#include <vector>
#include <fstream>
#include <algorithm>
#include <cmath>

const long double epsilon = 0.000000000001;

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
Type dotProduct (const Vector2D<Type> &firstVector, const Vector2D<Type> &secondVector) {
	return firstVector.x*secondVector.x + firstVector.y*secondVector.y;
}

template <typename Type>
Type crossProduct (const Vector2D<Type> &firstVector, const Vector2D<Type> &secondVector) {
	return firstVector.x*secondVector.y - firstVector.y*secondVector.x;
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


// SEGMENT:

template <typename Type>
struct Segment2D {
	Point2D<Type> begin, end;
	bool isNull;

	Segment2D (const Point2D<Type> &begin = Point2D<Type>(), const Point2D<Type> &end = Point2D<Type>()) {
		isNull = false;
		this->begin = begin;
		this->end = end;
	}
};

template <typename Type>
istream& operator >> (istream &is, Segment2D<Type> &segment) {
	return is >> segment.begin >> segment.end;
}

template <typename Type>
bool operator == (const Segment2D<Type> &firstSegment, const Segment2D<Type> &secondSegment) {
	return (firstSegment.begin == secondSegment.begin && firstSegment.end == secondSegment.end);
}


// BOX:

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
	Vector2D<Type> firstVector = segment.end - segment.begin;
	Vector2D<Type> secondVector = point - segment.begin;
	Point2D<Type> leftCorner(min<Type>(segment.begin.x, segment.end.x), min<Type>(segment.begin.y, segment.end.y));
	Point2D<Type> rightCorner(max<Type>(segment.begin.x, segment.end.x), max<Type>(segment.begin.y, segment.end.y));
	Box2D<Type> box = Box2D<Type>(leftCorner, rightCorner);
	return abs(crossProduct(firstVector, secondVector)) <= epsilon && isPointInBox(point, box);
}


template <typename Type>
bool getSegmentsIntersection (const Segment2D<Type> &segment1, const Segment2D<Type> &segment2, Segment2D<Type> &intersection) {
	if (segment1.begin == segment1.end && segment2.begin == segment2.end) {
		if (segment1 == segment2) {
			intersection = segment1;
			return true;
		}

		else
			return false;
	}

	else if (segment1.begin == segment1.end && segment2.begin != segment2.end) {
		if (isPointOnSegment(segment1.begin, segment2)) {
			intersection = segment1;
			return true;
		}

		else
			return false;
	}

	else if (segment1.begin != segment1.end && segment2.begin == segment2.end) {
		if (isPointOnSegment(segment2.begin, segment1)) {
			intersection = segment2;
			return true;
		}

		else
			return false;
	}

	else if (abs(crossProduct(segment1.end - segment1.begin, segment2.end - segment2.begin)) <= epsilon) {
		vector<Point2D<Type> > points;
		points.push_back(segment1.begin);
		points.push_back(segment1.end);
		points.push_back(segment2.begin);
		points.push_back(segment2.end);
		sort(points.begin(), points.end());

		Segment2D<Type> outerSegment(points[0], points[3]);
		Segment2D<Type> innerSegment(points[1], points[2]);
		Segment2D<Type> leftSegment = segment1;
		Segment2D<Type> rightSegment = segment2;

		if (min<Point2D<Type> >(leftSegment.begin, leftSegment.end) >= min<Point2D<Type> >(rightSegment.begin, rightSegment.end))
			swap(leftSegment, rightSegment);

		if (max<Point2D<Type> >(leftSegment.begin, leftSegment.end) >= min<Point2D<Type> >(rightSegment.begin, rightSegment.end) && isPointOnSegment(innerSegment.begin, outerSegment) && isPointOnSegment(innerSegment.end, outerSegment)) {
			intersection = innerSegment;
			return true;
		}

		else
			return false;
	}

	else {
		// AC
		Vector2D<Type> vec1 = segment2.begin - segment1.begin;
		// AD
		Vector2D<Type> vec2 = segment2.end - segment1.begin;
		// AB 
		Vector2D<Type> vec3 = segment1.end - segment1.begin;
		
		// C
		Type cross1 = crossProduct(vec1, vec3);
		// D
		Type cross2 = crossProduct(vec2, vec3);


		// CA
		Vector2D<Type> vec4 = segment1.begin - segment2.begin;
		// CB
		Vector2D<Type> vec5 = segment1.end - segment2.begin;
		// CD 
		Vector2D<Type> vec6 = segment2.end - segment2.begin;
		
		// A
		Type cross3= crossProduct(vec4, vec6);
		// B
		Type cross4 = crossProduct(vec5, vec6);
		
		if (cross1 * cross2 <= 0 && cross3 * cross4 <= 0) {
			Point2D<Type> point = segment2.begin + abs(cross1) / abs(cross1 - cross2) * (segment2.end - segment2.begin);
			intersection = Segment2D<Type>(point, point);
			return true;
		}

		else
			return false;
	}
}


int main () {
	unsigned segmentsCount;
	cin >> segmentsCount;
	vector<Segment2D<long double> > segments(segmentsCount);

	for (Segment2D<long double> &segment: segments)
		cin >> segment;

	unsigned count = 0;

	for (unsigned i = 0; i < segmentsCount; i ++) {
		for (unsigned j = i + 1; j < segmentsCount; j ++) {
			Segment2D<long double> intersection;
			
			if (getSegmentsIntersection(segments[i], segments[j], intersection))
				count ++;
		}
	}

	cout << count << endl;

	return 0;
}
