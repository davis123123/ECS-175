#include "Point.h"
#include <cmath>

int Point::xInt() const {
	return round(x);
}

int Point::yInt() const {
	return round(y);
}

Point& Point::operator+=(const Point& r) {
	x += r.x;
	y += r.y;
	return *this;
}

Point& Point::operator-=(const Point& r) {
	x -= r.x;
	y -= r.y;
	return *this;
}

Point operator+(const Point& l, const Point& r) {
	Point res = l;
	res += r;
	return res;
}

Point operator-(const Point& l, const Point& r) {
	Point res = l;
	res -= r;
	return res;
}

Point operator*(const Point& l, float a) {
	Point res = l;
	res.x = res.x * a;
	res.y = res.y * a;
	return res;
}

Point operator/(const Point& l, float a) {
	Point res = l;
	res.x = res.x / a;
	res.y = res.y / a;
	return res;
}

Point operator*(float a, const Point& l) {
	Point res = l;
	res.x = res.x * a;
	res.y = res.y * a;
	return res;
}

Point operator/(float a, const Point& l) {
	Point res = l;
	res.x = res.x / a;
	res.y = res.y / a;
	return res;
}
