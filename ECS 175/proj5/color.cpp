#include "color.h"

Color& Color::operator+=(const Color& rhs) {
	r += rhs.r;
	g += rhs.g;
	b += rhs.b;
	return *this;
}

Color& Color::operator-=(const Color& rhs) {
	r -= rhs.r;
	g -= rhs.g;
	b -= rhs.b;
	return *this;
}

Color& Color::operator*=(const Point& rhs) {
	r *= rhs.x;
	g *= rhs.y;
	b *= rhs.z;
	return *this;
}

Color& Color::operator*=(float a) {
	r *= a;
	g *= a;
	b *= a;
	return *this;
}

Color& Color::operator/=(float a) {
	r /= a;
	g /= a;
	b /= a;
	return *this;
}

Color operator+(const Color& l, const Color& rhs) {
	Color ret = l;
	ret += rhs;
	return ret;
}

Color operator-(const Color& l, const Color& rhs) {
	Color ret = l;
	ret -= rhs;
	return ret;
}

Color operator*(const Color& l, const Point& r) {
	Color ret = l;
	ret *= r;
	return ret;
}

Color operator*(const Color& l, float a) {
	Color ret = l;
	ret *= a;
	return ret;
}

Color operator/(const Color& l, float a) {
	Color ret = l;
	ret /= a;
	return ret;
}

istream& operator>>(istream& in, Color& p) {
	in >> p.r >> p.g >> p.b;
	return in;
}
ostream& operator<<(ostream& out, const Color& p) {
	out << p.r << ' ' << p.g << ' ' << p.b;
	return out;
}
