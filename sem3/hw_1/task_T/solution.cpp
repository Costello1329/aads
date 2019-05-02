#include <iostream>
#include <vector>
#include <fstream>
#include <algorithm>
#include <set>
#include <iomanip>
#include <cmath>

const long double epsilon = 0.000000001;
const long double pi = 3.14159265359;

using namespace std;

template <typename Type>
Type abs (const Type &number) {
	return number >= 0 ? number : - number;
}

template <typename Type>
int sign (const Type &number) {
	if (number == 0)
		return 0;

	else
		return number > 0 ? 1 : -1;
}


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
ostream& operator << (ostream &os, const Segment2D<Type> &segment) {
	return os << segment.begin << " -> " << segment.end;
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


// TRIANGLE:

template <typename Type>
struct Triangle2D {
	Point2D<Type> a, b, c;

	Triangle2D () {}

	Triangle2D (const Point2D<Type> &point1, const Point2D<Type> &point2, const Point2D<Type> &point3) {
		a = point1;
		b = point2;
		c = point3;
	}
};

template <typename Type>
istream& operator >> (istream &is, Triangle2D<Type> &triangle) {
	return is >> triangle.a >> triangle.b >> triangle.c;
}


// FUNCTIONS:

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
	return abs<Type>(crossProduct(firstVector, secondVector)) <= epsilon && isPointInBox(point, box);
}

template <typename Type>
bool getSegmentsIntersection (const Segment2D<Type> &segment1, const Segment2D<Type> &segment2, Segment2D<Type> &intersection) {
	if (segment1.begin == segment1.end && segment2.begin == segment2.end) {
		if (segment1 == segment2) {
			intersection = segment1;
			return true;
		}
	}

	else if (segment1.begin == segment1.end && segment2.begin != segment2.end) {
		if (isPointOnSegment(segment1.begin, segment2)) {
			intersection = segment1;
			return true;
		}
	}

	else if (segment1.begin != segment1.end && segment2.begin == segment2.end) {
		if (isPointOnSegment(segment2.begin, segment1)) {
			intersection = segment2;
			return true;
		}
	}

	else if (abs(crossProduct(segment1.end - segment1.begin, segment2.end - segment2.begin)) < epsilon) {
		vector<Point2D<Type> > points;
		points.push_back(segment1.begin);
		points.push_back(segment1.end);
		points.push_back(segment2.begin);
		points.push_back(segment2.end);
		sort(points.begin(), points.end());

		Segment2D<Type> outerSegment(points[0], points[3]);
		Segment2D<Type> innerSegment(points[1], points[2]);

		if (max<Point2D<Type> >(segment1.begin, segment1.end) >= min<Point2D<Type> >(segment2.begin, segment2.end) && isPointOnSegment(innerSegment.begin, outerSegment) && isPointOnSegment(innerSegment.end, outerSegment)) {
			intersection =  innerSegment;
			return true;
		}
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
			Point2D<Type> point = segment2.begin + abs<Type>(cross1) / abs<Type>(cross1 - cross2) * (segment2.end - segment2.begin);
			intersection = Segment2D<Type>(point, point);
			return true;
		}
	}

	return false;
}

template <typename Type>
long double getTriangleSurface (const Triangle2D<Type> &triangle) {
	Vector2D<Type> ab = triangle.b - triangle.a;
	Vector2D<Type> ac = triangle.c - triangle.a;
	return abs(crossProduct(ab, ac)) / 2;
}

template <typename Type>
bool isPointInTriangle (const Point2D<Type> &point, const Triangle2D<Type> &triangle) {
	long double surface = getTriangleSurface(triangle);
	long double surface1 = getTriangleSurface(Triangle2D<Type>(point, triangle.a, triangle.b));
	long double surface2 = getTriangleSurface(Triangle2D<Type>(point, triangle.b, triangle.c));
	long double surface3 = getTriangleSurface(Triangle2D<Type>(point, triangle.c, triangle.a));

	return abs(surface1 + surface2 + surface3 - surface) < epsilon;
}

struct pointComparator {
	template <typename Type>
	bool operator() (const Point2D<Type> &firstPoint, const Point2D<Type> &secondPoint) const {
		return firstPoint.x < secondPoint.x || (firstPoint.x == secondPoint.x && firstPoint.y < secondPoint.y);
	}
};

template <typename Type>
long double countPolarAngle (const Vector2D<Type> &vector) {
	return atan2(vector.y, vector.x);
}

template <typename Type>
long double getTwoTrianglesIntersectionSurface (const Triangle2D<Type> &triangle1, const Triangle2D<Type> &triangle2) {
	if (getTriangleSurface(triangle1) < epsilon || getTriangleSurface(triangle2) < epsilon)
		return 0;

	vector<Segment2D<Type> > segments1;
	vector<Segment2D<Type> > segments2;
	vector<Point2D<Type> > points1;
	vector<Point2D<Type> > points2;
	segments1.push_back(Segment2D<Type>(triangle1.a, triangle1.b));
	segments1.push_back(Segment2D<Type>(triangle1.b, triangle1.c));
	segments1.push_back(Segment2D<Type>(triangle1.c, triangle1.a));
	segments2.push_back(Segment2D<Type>(triangle2.a, triangle2.b));
	segments2.push_back(Segment2D<Type>(triangle2.b, triangle2.c));
	segments2.push_back(Segment2D<Type>(triangle2.c, triangle2.a));
	points1.push_back(triangle1.a);
	points1.push_back(triangle1.b);
	points1.push_back(triangle1.c);
	points2.push_back(triangle2.a);
	points2.push_back(triangle2.b);
	points2.push_back(triangle2.c);


	set<Point2D<Type>, pointComparator> polygon;


	for (const Segment2D<Type> &segment1: segments1) {
		for (const Segment2D<Type> &segment2: segments2) {
			Segment2D<Type> segment;

			if (getSegmentsIntersection(segment1, segment2, segment)) {
				//cout << segment1 << " & " << segment2 << " = " << segment << endl;
				polygon.insert(segment.begin);
				polygon.insert(segment.end);
			}
		}
	}


	for (const Point2D<Type> &point1: points1)
		if (isPointInTriangle(point1, triangle2))
			polygon.insert(point1);

	for (const Point2D<Type> &point2: points2)
		if (isPointInTriangle(point2, triangle1))
			polygon.insert(point2);



	//for (const Point2D<Type> &point: polygon)
	//	cout << point << endl;



	if (polygon.size() < 3)
		return 0;


	vector<Point2D<Type> > sorted;
	
	for (const Point2D<Type> &point: polygon)
		sorted.push_back(point);

	Point2D<Type> lowestLeftPoint = sorted[0];
	
	for (const Point2D<Type> &point: sorted)
		if (point.x < lowestLeftPoint.x || (point.x == lowestLeftPoint.x && point.y < lowestLeftPoint.y))
			lowestLeftPoint = point;


	sort(sorted.begin(), sorted.end(), [lowestLeftPoint](const Point2D<Type> &firstPoint, const Point2D<Type> &secondPoint){
		if (firstPoint == lowestLeftPoint)
			return true;

		else if (secondPoint == lowestLeftPoint)
			return false;
		
		else
			return countPolarAngle(Vector2D<Type>(firstPoint - lowestLeftPoint)) < countPolarAngle(Vector2D<Type>(secondPoint - lowestLeftPoint));
	});


	long double surface = 0;

	for (unsigned i = 0; i < sorted.size() - 2; i ++) {
		long double delta = getTriangleSurface(Triangle2D<Type>(lowestLeftPoint, sorted[i + 1], sorted[i + 2]));
		surface += delta;
		//cout << delta << endl;
	}

	return surface;
}



int main () {
	Triangle2D<long double> triangle1, triangle2;
	cin >> triangle1 >> triangle2;

	cout << fixed << setprecision(10);
	cout << getTriangleSurface(triangle1) + getTriangleSurface(triangle2) - getTwoTrianglesIntersectionSurface(triangle1, triangle2) << endl;

	return 0;
}
