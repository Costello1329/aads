#include <algorithm>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>
#include <cmath>
#include <list>
#include <set>

using namespace std;


// Vector2D: --------------------------------------------------


template <typename Type>
struct Vector2D {
	Type x, y;

	Vector2D (const Type &x = 0, const Type &y = 0) {
		this->x = x;
		this->y = y;
	}

	double length () const {
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


// Point2D: --------------------------------------------------:


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


// Segment2D: --------------------------------------------------


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


// Box2D: --------------------------------------------------


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
	return abs(crossProduct(firstVector, secondVector)) <= 0.000000001f && isPointInBox(point, box);
}


// Scan Line: --------------------------------------------------


template <typename Type, class Element>
struct ScanEvent {
	Type coordinate;
	Element element;
	enum EventType {addition = 0, query = 1, deletion = 2};
	EventType eventType;
	size_t id;
	
	explicit ScanEvent (const Type &coordinate, const Element &element, const EventType &eventType) : coordinate(coordinate), element(element), eventType(eventType) {}
};

template <typename Type, class Element>
struct ScanEventCompare {
	bool operator() (const ScanEvent<Type, Element> &firstEvent, const ScanEvent<Type, Element> &secondEvent) const {
		if (firstEvent.coordinate == secondEvent.coordinate)
			return firstEvent.eventType < secondEvent.eventType;

		return firstEvent.coordinate < secondEvent.coordinate;
	}
};

template <typename Type, class Element, class ElementCompare>
class ScanLine {
	private:
		multiset<ScanEvent<Type, Element>, ScanEventCompare<Type, Element> > eventsSet;
		typename set<ScanEvent<Type, Element>, ScanEventCompare<Type, Element> >::iterator iter;

		set<Element, ElementCompare> elementsSet;

	public:
		explicit ScanLine () : iter(eventsSet.begin()) {}

		void addEvent (const ScanEvent<Type, Element> &event) {
			begin();
			eventsSet.insert(event);
		}

		void begin () {
			iter = eventsSet.begin();
			elementsSet.clear();
		}

		void move () {
			if (iter->eventType == ScanEvent<Type, Element>::addition)
				elementsSet.insert(iter->element);

			else if (iter->eventType == ScanEvent<Type, Element>::deletion)
				elementsSet.erase(iter->element);

			++ iter;
		}

		bool end () const {
			return iter == eventsSet.end();
		}

		const ScanEvent<Type, Element> &getNextEvent () const {
			return *iter;
		}

		const set<Element, ElementCompare> &getElements () const {
			return elementsSet;
		}
};


// Custom comparator of segments: --------------------------------------------------


template <typename Type>
struct SegmentCompare {
	private:
		Type getY (const Segment2D<Type> &segment, const Type &coordinate) const {
			if (segment.begin.x == segment.end.x)
				return min<Type>(segment.begin.y, segment.end.y);
		
			else
				return segment.begin.y + (segment.end.y - segment.begin.y) * (coordinate - segment.begin.x) / (segment.end.x - segment.begin.x);
		}

		bool isPoint (const Segment2D<Type> &segment) const {
			return segment.begin == segment.end;
		}
		
		bool isVertical (const Segment2D<Type> &segment) const {
			return segment.begin.x == segment.end.x;
		}

	public:
		bool operator () (const Segment2D<Type> &firstSegment, const Segment2D<Type> &secondSegment) const {
			// Если нам пришли 2 отрезка:
			if (!isPoint(firstSegment) && !isPoint(secondSegment)) {
				Point2D<Type> leftPoint = max<Point2D<Type> >(min<Point2D<Type> >(firstSegment.begin, firstSegment.end), min<Point2D<Type> >(secondSegment.begin, secondSegment.end));
				Point2D<Type> rightPoint = min<Point2D<Type> >(max<Point2D<Type> >(firstSegment.begin, firstSegment.end), max<Point2D<Type> >(secondSegment.begin, secondSegment.end));
	
				// Отрезки равны:
				if ((firstSegment.begin == secondSegment.begin && firstSegment.end == secondSegment.end) || (firstSegment.begin == secondSegment.end && firstSegment.end == secondSegment.begin))
					return false;
	
				// Нет общих точек:
				else if (firstSegment.begin != secondSegment.begin && firstSegment.begin != secondSegment.end && firstSegment.end != secondSegment.begin && firstSegment.end != secondSegment.end)
					return (getY(firstSegment, leftPoint.x) < getY(secondSegment, leftPoint.x)) && (getY(firstSegment, rightPoint.x) < getY(secondSegment, rightPoint.x));
	
				// Есть одна общая точка:
				else {
					// Случай 1:
					if (!isVertical(firstSegment) && !isVertical(secondSegment) && leftPoint != rightPoint) {
						if (getY(firstSegment, leftPoint.x) == getY(secondSegment, leftPoint.x))
							return getY(firstSegment, rightPoint.x) < getY(secondSegment, rightPoint.x);
	
						else
							return getY(firstSegment, leftPoint.x) < getY(secondSegment, leftPoint.x);
					}
	
					// Случай 2:
					else if (isVertical(firstSegment) xor isVertical(secondSegment)) {
						if (isVertical(firstSegment))
							return max<Type>(firstSegment.begin.y, firstSegment.end.y) == getY(secondSegment, firstSegment.begin.x);
	
						else
							return max<Type>(secondSegment.begin.y, secondSegment.end.y) != getY(firstSegment, secondSegment.begin.x);
					}
	
					// Случай 3:
					else if (!isVertical(firstSegment) && !isVertical(secondSegment) && leftPoint == rightPoint)
						return max<Type>(firstSegment.begin.x, firstSegment.end.x) == min<Type>(secondSegment.begin.x, secondSegment.end.x);
	
	
					// Случай 4:
					else
						return max<Point2D<Type> >(firstSegment.begin, firstSegment.end) < max<Point2D<Type> >(secondSegment.begin, secondSegment.end);
				}
			}
	
			// Если пришел 1 отрезок и 1 точка:
			else {
				const Segment2D<Type> segment = (isPoint(firstSegment) ? secondSegment : firstSegment);
				const Point2D<Type> point = (isPoint(firstSegment) ? firstSegment.begin : secondSegment.end);
	
				if (!isPointOnSegment(point, segment)) {
					if (isPoint(firstSegment))
						return point.y < getY(segment, point.x);
	
					else
						return !(point.y < getY(segment, point.x));
				}
	
				else
					return !isPoint(firstSegment);
			}
		}
};


// Reading input: --------------------------------------------------



template <typename Type>
class Input {
	private:
		vector<Segment2D<Type> > polygon;
		vector<Point2D<Type> > points;

		void readPolygon (istream &is) {
			polygon.clear();
			size_t verticesCount;
			is >> verticesCount;
		
			vector<Point2D<Type> > points;
		
			for (size_t i = 0; i < verticesCount; i ++) {
				Point2D<Type> point;
				is >> point.x >> point.y;
		
				if (points.size() >= 2) {
					Vector2D<Type> vec1 = points[points.size() - 1] - points[points.size() - 2];
					Vector2D<Type> vec2 = point - points[points.size() - 1];
			
					if (crossProduct(vec1, vec2) == 0)
						points.pop_back();
				}
			
				points.push_back(point);
			}
		
			if (points.back() == points.front())
				points.pop_back();
		
			for (size_t i = 0; i < points.size() - 1; i ++)
				polygon.push_back(Segment2D<Type>(points[i], points[i + 1]));
		
			polygon.push_back(Segment2D<Type>(points.back(), points[0]));
		}

		void readPoints (istream &is) {
			points.clear();
			size_t pointsCount;
			is >> pointsCount;
		
			for (size_t i = 0; i < pointsCount; i ++) {
				Point2D<Type> point;
				is >> point.x >> point.y;
				points.push_back(point);
			}
		}

		void reorientPolygon () {
			Type crossProductSum = 0;
		
			for (size_t i = 0; i < polygon.size(); i ++) {
				Segment2D<Type> seg = polygon[i];
				Vector2D<Type> vec1 = seg.begin - Point2D<Type>(0, 0);
				Vector2D<Type> vec2 = seg.end - Point2D<Type>(0, 0);
				crossProductSum += crossProduct(vec1, vec2);
			}
		
			if (crossProductSum > 0) {
				reverse(polygon.begin(), polygon.end());
		
				for (Segment2D<Type> &seg: polygon)
					swap(seg.begin, seg.end);
			}
		}

	public:
		explicit Input (istream &is) {
			polygon.clear();
			points.clear();
			readPolygon(is);
			readPoints(is);
			reorientPolygon();
		}

		const vector<Segment2D<Type> > &getPolygon () const {
			return polygon;
		}

		const vector<Point2D<Type> > &getPoints () const {
			return points;
		}
};

class Output {
	private:
		enum outputType {Border, Inner, Outer, Undefined};
		vector<outputType> queryAnswers;

	public:
		typedef outputType OutputType;
		explicit Output () {
			queryAnswers.clear();
		};

		void set (const size_t &index, const OutputType &type) {
			if (queryAnswers.size() <= index)
				queryAnswers.resize(index + 1, Undefined);

			queryAnswers[index] = type;
		}

		void makeOutput (ostream &os) const {
			for (const outputType &el: queryAnswers) {
				if (el == outputType::Border)
					os << "BORDER\n";
		
				else if (el == outputType::Inner)
					os << "INSIDE\n";
		
				else if (el == outputType::Outer)
					os << "OUTSIDE\n";
			}
		}
};

void processTest (istream &is, ostream &os) {
	typedef double Type;
	typedef ScanLine<Type, Segment2D<Type>, SegmentCompare<Type> > ScanLine;
	typedef ScanEvent<Type, Segment2D<Type> > ScanEvent;


	Input<Type> input(is);


	ScanLine scanLine;

	for (const Segment2D<Type> &seg: input.getPolygon()) {
		ScanEvent additionEvent = ScanEvent(min<Type>(seg.begin.x, seg.end.x), seg, ScanEvent::EventType::addition);
		ScanEvent deletionEvent = ScanEvent(max<Type>(seg.begin.x, seg.end.x), seg, ScanEvent::EventType::deletion);
		scanLine.addEvent(additionEvent);
		scanLine.addEvent(deletionEvent);
	}

	size_t id = 0;

	for (const Point2D<Type> &point: input.getPoints()) {
		ScanEvent queryEvent = ScanEvent(point.x, Segment2D<Type>(point, point), ScanEvent::EventType::query);
		queryEvent.id = id ++;
		scanLine.addEvent(queryEvent);
	}


	Output output;
	scanLine.begin();

	while (!scanLine.end()) {
		ScanEvent event = scanLine.getNextEvent();
		scanLine.move();

		if (event.eventType == ScanEvent::EventType::query) {
			if (scanLine.getElements().size() != 0 && scanLine.getElements().lower_bound(event.element) != scanLine.getElements().begin()) {
				Segment2D<Type> boundry = *prev(scanLine.getElements().lower_bound(event.element));
				Vector2D<Type> firstVector = boundry.end - boundry.begin;
				Vector2D<Type> secondVector = Point2D<Type>(0, 100) - Point2D<Type>(0, 0);
				
				if (isPointOnSegment(event.element.begin, boundry))
					output.set(event.id, Output::OutputType::Border);

				else if (crossProduct(firstVector, secondVector) > 0)
					output.set(event.id, Output::OutputType::Outer);

				else
					output.set(event.id, Output::OutputType::Inner);
			}

			else
				output.set(event.id, Output::OutputType::Outer);
		}
	}

	output.makeOutput(os);
}

int main () {
	size_t tests;
	cin >> tests;
	
	for (size_t i = 0; i < tests; i ++) {
		processTest(cin, cout);
		cout << endl;
	}

	return 0;
}
