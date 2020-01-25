#include "base.h"

using namespace std;

istream& operator>>(istream& in, Point& p) {
	in >> p.x >> p.y >> p.z;
	return in;
}

istream& operator>>(istream& in, Point3i& p) {
	in >> p.x >> p.y >> p.z;
	return in;
}


ostream& operator<<(ostream& out, const Point& p) {
	out << p.x << ' ' << p.y << ' ' << p.z;
	return out;
}

ostream& operator<<(ostream& out, const Point3i& p) {
	out << p.x << ' ' << p.y << ' ' << p.z;
	return out;
}


int Point::xInt() const {
	return round(x);
}

int Point::yInt() const {
	return round(y);
}

int Point::zInt() const {
	return round(z);
}

float Point::length() const {
	return sqrt(x * x + y * y + z * z);
}

Point& Point::operator+=(const Point& r) {
	x += r.x;
	y += r.y;
	z += r.z;
	return *this;
}

Point& Point::operator-=(const Point& r) {
	x -= r.x;
	y -= r.y;
	z -= r.z;
	return *this;
}

Point& Point::operator*=(float a) {
	x *= a;
	y *= a;
	z *= a;
	return *this;
}
Point& Point::operator/=(float a) {
	x /= a;
	y /= a;
	z /= a;
	return *this;
}

Point operator+(const Point& l, const Point& r) {
	Point ret = l;
	ret += r;
	return ret;
}

Point operator-(const Point& l, const Point& r) {
	Point ret = l;
	ret -= r;
	return ret;
}

Point operator*(const Point& l, const Point& r) {
	Point ret = l;
	ret.x *= r.x;
	ret.y *= r.y;
	ret.z *= r.z;
	return ret;
}

Point operator*(const Point& l, float a) {
	Point ret = l;
	ret *= a;
	return ret;
}

Point operator/(const Point& l, float a) {
	Point ret = l;
	ret /= a;
	return ret;
}

Point operator*(float a, const Point& l) {
	Point ret = l;
	ret *= a;
	return ret;
}

Point operator/(float a, const Point& l) {
	Point ret = l;
	ret /= a;
	return ret;
}
