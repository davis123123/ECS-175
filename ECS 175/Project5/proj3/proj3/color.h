#ifndef COLOR_H
#define COLOR_H

#include <iostream>
#include "base.h"

using namespace std;

class Color {
public:
	float r, g, b;
	Color() :r(0.0), g(0.0), b(0.0) {}
	Color(float vr, float vg, float vb) :r(vr), g(vg), b(vb) {}
	Color& operator+=(const Color& rhs);
	Color& operator-=(const Color& rhs);
	Color& operator*=(const Point& rhs);
	Color& operator*=(float a);
	Color& operator/=(float a);
};

Color operator+(const Color& l, const Color& rhs);
Color operator-(const Color& l, const Color& rhs);
Color operator*(const Color& l, const Point& r);
Color operator*(const Color& l, float a);
Color operator/(const Color& l, float a);

istream& operator>>(istream& in, Color& p);
ostream& operator<<(ostream& out, const Color& p);

#endif
