#ifndef POINT_H
#define POINT_H

class Point {
public:
	float x, y;
	Point() :x(0.0), y(0.0) {}
	Point(float vx, float vy) :x(vx), y(vy) {}
	int xInt() const;
	int yInt() const;
	Point& operator+=(const Point& r);
	Point& operator-=(const Point& r);
};

Point operator+(const Point& l, const Point& r);
Point operator-(const Point& l, const Point& r);
Point operator*(const Point& l, float a);
Point operator*(float a, const Point& l);
Point operator/(const Point& l, float a);
Point operator/(float a, const Point& l);

#endif
