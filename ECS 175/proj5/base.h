#ifndef BASE_H
#define BASE_H

#include <GL/glut.h>
#include <cmath>
#include <iostream>

#define PI 3.141593
#define ZERO(x) (-0.00001 <= x && x < 0.00001)
#define NEG(x) (x <= -0.00001)
#define POS(x) (0.00001 <= x)

class Point2i {
public:
	int x, y;
	Point2i() :x(0), y(0) {}
	Point2i(int vx, int vy) :x(vx), y(vy) {}
};

class Point3i {
public:
	int x, y, z;
	Point3i() :x(0), y(0), z(0) {}
	Point3i(int vx, int vy, int vz) :x(vx), y(vy), z(vz) {}
};

class Point {
public:
	float x, y, z;
	Point() :x(0.0), y(0.0), z(0.0) {}
	Point(float vx, float vy, float vz) :x(vx), y(vy), z(vz) {}
	int xInt() const;
	int yInt() const;
	int zInt() const;
	float length() const;
	Point& norm();
	void show() const;
	Point& operator+=(const Point& r);
	Point& operator-=(const Point& r);
	Point& operator*=(float a);
	Point& operator/=(float a);
};

inline Point& Point::norm() {
	(*this) /= length();
	return *this;
}

inline void Point::show() const {
	glVertex3f(x, y, z);
}

inline Point cross(const Point& l, const Point& r) {
	Point ret;
	ret.x = l.y * r.z - l.z * r.y;
	ret.y = l.z * r.x - l.x * r.z;
	ret.z = l.x * r.y - l.y * r.x;
	return ret;
}

inline float dot(const Point& l, const Point& r) {
	return l.x * r.x + l.y * r.y + l.z * r.z;
}

Point operator+(const Point& l, const Point& r);
Point operator-(const Point& l, const Point& r);
Point operator*(const Point& l, const Point& r);
Point operator*(const Point& l, float a);
Point operator/(const Point& l, float a);
Point operator*(float a, const Point& l);
Point operator/(float a, const Point& l);

std::istream& operator>>(std::istream& in, Point& p);
std::istream& operator>>(std::istream& in, Point3i& p);
std::ostream& operator<<(std::ostream& out, const Point& p);
std::ostream& operator<<(std::ostream& out, const Point3i& p);

#endif
