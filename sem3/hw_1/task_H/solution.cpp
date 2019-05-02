#include <iostream>
#include <iomanip>
#include <cmath>

const long double epsilon = 0.000000001f;
using namespace std;



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
struct Segment2D {
	Vector2D<Type> start, finish;

	Segment2D (const Vector2D<Type> &start = Vector2D<Type>(), const Vector2D<Type> &finish = Vector2D<Type>()) {
		this->start = start;
		this->finish = finish;
	}
};

template <typename Type>
struct Box2D {
	Vector2D<Type> leftCorner, rightCorner;

	Box2D (const Vector2D<Type> &leftCorner = Vector2D<Type>(), const Vector2D<Type> &rightCorner = Vector2D<Type>()) {
		this->leftCorner = leftCorner;
		this->rightCorner = rightCorner;
	}
};


template <typename Type>
Type crossProduct (const Vector2D<Type> &firstVector, const Vector2D<Type> &secondVector) {
	return firstVector.x*secondVector.y - firstVector.y*secondVector.x;
}

template <typename Type>
Vector2D<Type> operator - (const Vector2D<Type> &firstVector, const Vector2D<Type> &secondVector) {
	return Vector2D<Type>(firstVector.x - secondVector.x, firstVector.y - secondVector.y);
}

template <typename Type>
bool isPointInBox (const Vector2D<Type> &point, const Box2D<Type> &box) {
	return (box.leftCorner.x <= point.x && point.x <= box.rightCorner.x) && (box.leftCorner.y <= point.y && point.y <= box.rightCorner.y);
}

template <typename Type>
bool isPointOnSegment (const Vector2D<Type> &point, const Segment2D<Type> &segment) {
	Vector2D<long double> firstVector = segment.finish - segment.start;
	Vector2D<long double> secondVector = point - segment.start;
	Vector2D<long double> leftCorner(min<long double>(segment.start.x, segment.finish.x), min<long double>(segment.start.y, segment.finish.y));
	Vector2D<long double> rightCorner(max<long double>(segment.start.x, segment.finish.x), max<long double>(segment.start.y, segment.finish.y));
	Box2D<long double> box = Box2D<long double>(leftCorner, rightCorner);
	return abs(crossProduct(firstVector, secondVector)) < epsilon && isPointInBox(point, box);
}


int main () {
	Vector2D<long double> point;
	cin >> point.x >> point.y;

	Segment2D<long double> segment;
	cin >> segment.start.x >> segment.start.y >> segment.finish.x >> segment.finish.y;

	cout << (isPointOnSegment(point, segment) ? "YES" : "NO") << endl;

	return 0;
}
