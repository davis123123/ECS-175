#ifndef OBJECT_H
#define OBJECT_H

#include "math.h"
#include "base.h"
#include "color.h"
#include <vector>

using namespace std;

extern float iA, iL, kK;
extern Point kA, kD, kS;
extern int bigN;
extern float bigK;
extern float kT, kR;
extern float eta;
extern Point lightSource;

class Line {
public:
	Point start;
	Point u;
	bool inside;
public:
	Line(const Point& start_, const Point& u_, bool inside_) :
		start(start_), u(u_), inside(inside_) {}
	Line() :inside(false) {}
};

class Prim {
public:
	bool transparent;
	Color color;
public:
	virtual bool intersect(const Line& l, Point& result) = 0;
	virtual Point normal(const Point& p) = 0;
};

class Poly : public Prim {
public:
	vector<Point> v;
	Point n;
public:
	bool intersect(const Line& l, Point& result);
	Point normal(const Point& p);
};

class Circ : public Prim {
public:
	Point center;
	float radius;
public:
	bool intersect(const Line& l, Point& result);
	Point normal(const Point& p);
};

class Quad : public Prim {
public:
	float A, B, C, D, E, F, G, H, I, J;
public:
	Point normal(const Point& p);
	bool intersect(const Line& l, Point& result);
};

inline void makePixel(int x, int y, const Color& c, float* pixels, int resolution) {
	if (0 <= x && x < resolution && 0 <= y && y < resolution) {
		pixels[3 * (y * resolution + x)] = c.r;
		pixels[3 * (y * resolution + x) + 1] = c.g;
		pixels[3 * (y * resolution + x) + 2] = c.b;
	}
}
inline void normalize(float* pixels, int resolution) {
	float max = 0.0;
	for (int i = 0; i < pow(resolution , 2) * 3; i++) {
		if (max < pixels[i])
			max = pixels[i];
	}
	if (0.0 < max) {
		for (int i = 0; i < pow(resolution, 2) * 3; i++)
			pixels[i] /= max;
	}
}

Poly* newTriangle(const Point& v1, const Point& v2, const Point& v3, const Point& normal);
Poly* newSquare(const Point& v1, const Point& v2, const Point& v3, const Point& v4, const Point& normal);
Quad* newEllip(const Point& center, float a, float b, float c);
Quad* newSphere(const Point& center, float radius);
Point reflect(const Point& u, const Point& normal);
Point phong(const Point& point, const Point& ref, const Point& normal, const vector<Prim*>& primitives);
bool intersect(const Line& line, const vector<Prim*>& primitives, Point& result, int& resultId);
Color light(const Line& line, const vector<Prim*>& primitives, int depth);

#endif
