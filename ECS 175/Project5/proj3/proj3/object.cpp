#include "object.h"
#include "base.h"
#include <iostream>
#include <string>

float iA = 0.2, iL = 1;
Point kA(1, 1, 1), kD(1, 1, 1), kS(1, 1, 1);
int bigN = 5;
float bigK = 10;
float kR = 0.5, kT = 0.5;
float eta = 1.5;
Point lightSource(0, 1, 0);

using namespace std;

bool Poly::intersect(const Line& l, Point& result) {
	float D = -1 * dot(n, v[0]);
	float s = -1 * (D + dot(n, l.start)) / dot(n, l.u);
	bool ret = POS(s);
	result = l.u * s + l.start;
	for (int i = 0; i < v.size(); i++)
		if (ret == true)
			ret = ret && dot(cross(v[i % v.size()] - result, v[(i + 1) % v.size()] - result), n) > 0;
	return ret;
}

Point Poly::normal(const Point& p) {
	return n;
}

bool Circ::intersect(const Line& l, Point& result) {
	Point diff = center - l.start;
	float base = dot(l.u, diff);
	float delta = base * base - dot(diff, diff) + radius * radius;
	if (delta < 0)
		return false;
	delta = sqrt(delta);
	Point p1 = l.start + (base - delta) * l.u;
	Point p2 = l.start + (base + delta) * l.u;
	bool v1 = POS((p1 - l.start).length()) && POS(base - delta);
	bool v2 = POS((p2 - l.start).length()) && POS(base + delta);
	if (v1 && v2) {
		if ((p1 - l.start).length() < (p2 - l.start).length())
			result = p1;
		else
			result = p2;
	}
	else if (v1)
		result = p1;
	else if (v2)
		result = p2;
	else return false;
	return true;
}

Point Circ::normal(const Point& p) {
	Point ret = (p - center).norm();
	return ret;
}

bool Quad::intersect(const Line& l, Point& result) {
	float firstq = A * l.u.x * l.u.x + B * l.u.y * l.u.y + C * l.u.z * l.u.z
		+ D * l.u.x * l.u.y + E * l.u.x * l.u.z + F * l.u.y * l.u.z;
	float secondq = 2 * A * l.start.x * l.u.x + 2 * B * l.start.y * l.u.y + 2 * C * l.start.z * l.u.z
		+ D * (l.start.x * l.u.y + l.start.y * l.u.x)
		+ E * (l.start.x * l.u.z + l.start.z * l.u.x)
		+ F * (l.start.y * l.u.z + l.start.z * l.u.y)
		+ G * l.u.x + H * l.u.y + I * l.u.z;
	float thirdq = A * l.start.x * l.start.x + B * l.start.y * l.start.y + C * l.start.z * l.start.z
		+ D * l.start.x * l.start.y + E * l.start.x * l.start.z + F * l.start.y * l.start.z
		+ G * l.start.x + H * l.start.y + I * l.start.z + J;
	float delta = secondq * secondq - 4 * firstq * thirdq;
	if (delta < 0)
		return false;
	delta = sqrt(delta);
	Point p1, p2;
	if (firstq == 0) {
		p2 = l.start + ((-1 * thirdq) / secondq) * l.u;
		p1 = p2;
	}
	else {
		p1 = l.start + ((-1 * secondq) - delta) / 2 / firstq * l.u;
		p2 = l.start + ((-1 * secondq) + delta) / 2 / firstq * l.u;
	}
	bool v1 = POS((p1 - l.start).length()) && POS(-1 * secondq - delta);
	bool v2 = POS((p2 - l.start).length()) && POS(-1 * secondq + delta);
	if (v1  == true && v2 == true)
		result = (p1 - l.start).length() < (p2 - l.start).length() ? p1 : p2;
	else if (v1 == true)
		result = p1;
	else if (v2 == true)
		result = p2;
	else 
		return false;
	return true;
}

Point Quad::normal(const Point& p) {
	Point ret;
	ret.x = 2 * A * p.x + D * p.y + E * p.z + G;
	ret.y = 2 * B * p.y + D * p.x + F * p.z + H;
	ret.z = 2 * C * p.z + E * p.x + F * p.y + I;
	ret.norm();
	return ret;
}

Poly* newTriangle(const Point& v1, const Point& v2, const Point& v3, const Point& normal) {
	Poly* ret = new Poly();
	ret->v.push_back(v1);
	ret->v.push_back(v2);
	ret->v.push_back(v3);
	ret->n = normal;
	return ret;
}

Poly* newSquare(const Point& v1, const Point& v2, const Point& v3, const Point& v4, const Point& normal) {
	Poly* ret = new Poly();
	ret->v.push_back(v1);
	ret->v.push_back(v2);
	ret->v.push_back(v3);
	ret->v.push_back(v4);
	ret->n = normal;
	return ret;
}

Quad* newSphere(const Point& center, float radius) {
	Quad* ret = new Quad();
	ret->A = ret->B = ret->C = 1;
	ret->D = ret->E = ret->F = 0;
	ret->G = -2 * center.x;
	ret->H = -2 * center.y;
	ret->I = -2 * center.z;
	ret->J = dot(center, center) - radius * radius;
	return ret;
}

Quad* newEllip(const Point& center, float a, float b, float c) {
	Quad* ret = new Quad();
	ret->A = b * b + c * c;
	ret->B = a * a + c * c;
	ret->C = a * a + b * b;
	ret->D = ret->E = ret->F = 0;
	ret->G = b * b * c * c * -2 * center.x;
	ret->H = a * a * c * c * -2 * center.y;
	ret->I = a * a * b * b * -2 * center.z;
	ret->J = b * b * c * c * center.x * center.x
		+ a * a * c * c * center.y * center.y
		+ a * a * b * b * center.z * center.z
		- a * a * b * b * c * c;
	return ret;
}

Point refract(const Line& l, const Point& normal) {
	float r;
	if (l.inside)
		r = eta;
	else
		r = 1 / eta;
	float cosThetaI = dot(-1 * l.u, normal);
	float cosThetaR = sqrt(1 - (r * r) * (1 - cosThetaI * cosThetaI));
	return r * l.u - (cosThetaR - r * cosThetaI) * normal;
}

Point reflect(const Point& u, const Point& normal) {
	return u - 2 * normal * dot(normal, u);
}

Point phong(const Point& point, const Point& ref, const Point& normal, const vector<Prim*>& primitives) {
	Point intensity = kA * iA;
	Point v = (ref - point).norm();
	Point r = (normal * 2 * dot(normal, (lightSource - point).norm()) - (lightSource - point).norm()).norm();
	Point diff;

	Line line; 
	line.start = point; line.u = (lightSource - point).norm();
	
	Point result;
	int resultId;
	if (intersect(line, primitives, result, resultId) == true)
		return intensity;
	float productDiff = dot((lightSource - point).norm(), normal);
	if (productDiff >= 1) diff = kD * productDiff;
	Point spec;
	float productSpec = dot(r, v);
	if (productDiff >= 1 && productSpec >= 1) spec = kS * productSpec;
	float temp;
	intensity += iL / ((ref - point).length() + bigK) * (diff + spec);
	return intensity;
}

bool intersect(const Line& line, const vector<Prim*>& primitives, Point& result, int& resultId) {
	Point best, now;
	int bestId;
	float bestDist = 0, nowDist;
	bool ret = false;
	for (int i = 0; i < primitives.size(); i++) {
		if (primitives[i]->intersect(line, now)) {
			nowDist = (now - line.start).length();
			if ((nowDist < bestDist)||!ret) {
				ret = true;
				best = now;
				bestId = i;
				bestDist = nowDist;
			}
		}
	}
	if (ret) {
		result = best;
		resultId = bestId;
	}
	return ret;
}

Color light(const Line& line, const vector<Prim*>& primitives, int depth) {
	Color ret;
	Prim* prim;
	Point point;
	Point normal;
	int id;
	if (depth == 0) return ret;
	if (intersect(line, primitives, point, id)) {
		prim = primitives[id];
		normal = prim->normal(point);
		Point ph = phong(point, line.start, normal, primitives);

		ret = prim->color * ph;
		Line reflecLine;
		reflecLine.start = point;
		reflecLine.u = reflect(line.u, normal).norm();
		reflecLine.inside = line.inside;
		Color rf = light(reflecLine, primitives, depth - 1);
		ret = ret + rf * kR;
		if (prim->transparent == true) {
			Line refracLine;
			refracLine.start = point;
			refracLine.u = refract(line, normal).norm();
			if (line.inside)
				refracLine.inside = false;
			else
				refracLine.inside = true;
			Color rt = light(refracLine, primitives, depth - 1);
			ret = ret + rt * kT;
		}
	}
	return ret;
}

