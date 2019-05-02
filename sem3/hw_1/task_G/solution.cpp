#include <iostream>
#include <iomanip>
#include <cmath>

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
Type crossProduct (const Vector2D<Type> &firstVector, const Vector2D<Type> &secondVector) {
	return firstVector.x*secondVector.y - firstVector.y*secondVector.x;
}

template <typename Type>
Type dotProduct (const Vector2D<Type> &firstVector, const Vector2D<Type> &secondVector) {
	return firstVector.x*secondVector.x + firstVector.y*secondVector.y;
}

template <typename Type>
long double countAngleBetweenVectors (const Vector2D<Type> &firstVector, const Vector2D<Type> &secondVector) {
	return atan2(crossProduct(firstVector, secondVector), dotProduct(firstVector, secondVector));
}


int main () {
	Vector2D<long double> firstVector, secondVector;
	cin >> firstVector.x >> firstVector.y >> secondVector.x >> secondVector.y;
	long double angle = abs(countAngleBetweenVectors(firstVector, secondVector));
	cout << setprecision(6) << fixed << angle << endl;
	return 0;
}

