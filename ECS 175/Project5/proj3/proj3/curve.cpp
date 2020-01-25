#include "curve.h"
#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <GL/glut.h>

using namespace std;

int Curve::size() const {
	return polygon.polyP.size();
}

void Curve::set(int pos, const Point& p) {
	if (0 > pos && pos >= polygon.polyP.size())
		throw runtime_error("Index out of bound");
	else
		polygon.polyP[pos] = p;
		
}

void Curve::add(const Point& p) {
	polygon.polyP.push_back(p);
}

void Curve::insert(int pos, const Point& p) {
	if (0 > pos && pos >= polygon.polyP.size()) {
		throw runtime_error("Index out of bound");
	}
	else
		polygon.polyP.insert(polygon.polyP.begin() + pos, p);
		
}

void Curve::remove(int pos) {
	if (0 > pos && pos >= polygon.polyP.size()) {
		throw runtime_error("Index out of bound");
	}
	else
		polygon.polyP.erase(polygon.polyP.begin() + pos);
		
}

const Point& Curve::get(int pos) const {
	if (0 > pos && pos >= polygon.polyP.size())
		throw runtime_error("Index out of bound");
	else
		return polygon.polyP[pos];
}


void Bezier::dda(double xstart, double xend, double ystart, double yend) const {
	double ydiff = yend - ystart;
	double xdiff = xend - xstart;
	double step = 0;
	if (abs(xdiff) > abs(ydiff)) {
		step = abs(xdiff);
	}
	else {
		step = abs(ydiff);
	}

	double xinc = xdiff / step;
	double yinc = ydiff / step;
	if (yinc == 1) {
		yinc /= 10;
	}
	if (xinc == 1) {
		xinc /= 10;
	}
	double x = xstart;
	double y = ystart;
	for (double i = 0; i < xend && i < yend; i+=step/10) {
		x = x + xinc;
		y = y + yinc;

		glColor3f(255, 0, 0);  //sets the current drawing (foreground) color to blue
		glVertex2i(x, y); //set vertex
		glColor3f(255, 255, 255);
	}
}

void Bezier::show(float t) const {
	if (polygon.polyP.empty()) return;
	int time = polygon.polyP.size() - 1;
	vector<Point> last, now(polygon.polyP.begin(), polygon.polyP.end());
	while (time--) {
		last.assign(now.begin(), now.end());
		now.clear();
		for (int i = 0; i < last.size() - 1; i++)
			now.push_back(t * last[i] + (1 - t) * last[i + 1]);
	}
	glVertex2f(now.front().x, now.front().y);
}

void Bezier::show() const {
	vector<double> flatKnot;
	for (int i = 0; i < polygon.polyP.size() + 2; i++)
		flatKnot.push_back(i);
	float delta2 = (flatKnot.at(polygon.polyP.size()) - flatKnot.at(2 - 1)) / 1000;
	for (float u = flatKnot.at(2 - 1); u < flatKnot.at(polygon.polyP.size()); u += delta2)
		show2(u);
	for (float t = 0; t <= 1; t += 0.001)
		show(t);
}

void Bezier::show2(float u) const {
	if (polygon.polyP.size() == 0) return;
	vector<double> flatKnot;
	for (int i = 0; i < polygon.polyP.size() + 2; i++)
		flatKnot.push_back(i);
	int bigI2;
	bool found2 = false;
	for (bigI2 = 2 - 1; bigI2 < polygon.polyP.size(); bigI2++)
		if (flatKnot.at(bigI2) <= u && u < flatKnot.at(bigI2 + 1)) {
			found2 = true;
			break;
		}
	if (!found2) throw runtime_error("Knot out of range");
	vector<Point> last2;
	int beg2 = bigI2 - 2 + 1;
	int end2 = beg2 + 2;
	vector<Point> now2(
		polygon.polyP.begin() + beg2,
		polygon.polyP.begin() + end2);
	for (int r = 0; r < 2 - 1; r++) {
		last2.assign(now2.begin(), now2.end());
		now2.clear();
		for (int i = 0; i < last2.size() - 1; i++) {
			int leftIndex = bigI2 - 2 + i + r + 2;
			int rightIndex = bigI2 + 1 + i;
			float left = flatKnot.at(leftIndex);
			float right = flatKnot.at(rightIndex);
			now2.push_back(((right - u) * last2[i] + (u - left) * last2[i + 1]) / (right - left));
		}
	}
	glColor3f(255, 0, 0);
	glVertex2f(now2.front().x, now2.front().y);
	glColor3f(255, 255, 255);
}


int Bspline::knotSize() const {
	return knots.size();
}

void Bspline::setKnot(int pos, float k) {
	if (0 <= pos && pos < knots.size())
		knots[pos] = k;
	else
		throw runtime_error("Index out of bound");
}

float Bspline::getKnot(int pos) const {
	if (0 <= pos && pos < knots.size())
		return knots[pos];
	else
		throw runtime_error("Index out of bound");
}

void Bspline::addKnot() {
	float delta;
	if (knots.size() <= 1)
		delta = 1.0;
	else
		delta = (knots.back() - knots.front()) / (knots.size() - 1);
	knots.push_back(knots.size() ? knots.back() + delta : 0.0);
}

void Bspline::setK(int vk) {
	if (polygon.polyP.size() < 2 || (2 <= vk && vk <= polygon.polyP.size())) {
		float delta;
		while (k < vk) {
			addKnot();
			k++;
		}
		while (k > vk) {
			knots.pop_back();
			k--;
		}
	}
}

void Bspline::addK(int dk) {
	setK(k + dk);
}

int Bspline::getK() const {
	return k;
}

void Bspline::add(const Point& p) {
	Curve::add(p);
	addKnot();
}

void Bspline::insert(int pos, const Point& p) {
	Curve::insert(pos, p);
	addKnot();
}

void Bspline::remove(int pos) {
	Curve::remove(pos);
	knots.pop_back();
}

void Bspline::show(float u) const {
	if (polygon.polyP.size() == 0) return;
	int largeI;
	bool found = false;
	for (largeI = k - 1; largeI < polygon.polyP.size(); largeI++)
		if (knots[largeI] <= u && u < knots[largeI + 1]) {
			found = true;
			break;
		}
	if (!found) throw runtime_error("Knot out of range");
	vector<Point> lastP;
	int beg = largeI - k + 1;
	int end = beg + k;
	vector<Point> now(
		polygon.polyP.begin() + beg,
		polygon.polyP.begin() + end);
	for (int r = 0; r < k - 1; r++) {
		lastP.assign(now.begin(), now.end());
		now.clear();
		for (int i = 0; i < lastP.size() - 1; i++) {
			int leftIndex = largeI - k + i + r + 2;
			int rightIndex = largeI + 1 + i;
			float left = knots[leftIndex];
			float right = knots[rightIndex];
			now.push_back(((right - u) * lastP[i] + (u - left) * lastP[i + 1]) / (right - left));
		}
	}
	glVertex2f(now.front().x, now.front().y);
}

void Bspline::show2(float u) const {
	if (polygon.polyP.size() == 0) return;
	vector<double> flatKnot;
	for (int i = 0; i < polygon.polyP.size() + 2; i++)
		flatKnot.push_back(i);
	int bigI2;
	bool found2 = false;
	for (bigI2 = 2 - 1; bigI2 < polygon.polyP.size(); bigI2++)
		if (flatKnot.at(bigI2) <= u && u < flatKnot.at(bigI2 + 1)) {
			found2 = true;
			break;
		}
	if (!found2) throw runtime_error("Knot out of range");
	vector<Point> last2;
	int beg2 = bigI2 - 2 + 1;
	int end2 = beg2 + 2;
	vector<Point> now2(
		polygon.polyP.begin() + beg2,
		polygon.polyP.begin() + end2);
	for (int r = 0; r < 2 - 1; r++) {
		last2.assign(now2.begin(), now2.end());
		now2.clear();
		for (int i = 0; i < last2.size() - 1; i++) {
			int leftIndex = bigI2 - 2 + i + r + 2;
			int rightIndex = bigI2 + 1 + i;
			float left = flatKnot.at(leftIndex);
			float right = flatKnot.at(rightIndex);
			now2.push_back(((right - u) * last2[i] + (u - left) * last2[i + 1]) / (right - left));
		}
	}
	glColor3f(0, 0, 255);
	glVertex2f(now2.front().x, now2.front().y);
	glColor3f(255, 255, 255);
}

void Bspline::show() const {
	if (knots.size()) {
		float delta = (knots[polygon.polyP.size()] - knots[k - 1]) / 1000;
		
		for (float u = knots[k - 1]; u < knots[polygon.polyP.size()]; u += delta)
			show(u);

		vector<double> flatKnot;
		for (int i = 0; i < polygon.polyP.size() + 2; i++)
			flatKnot.push_back(i);
		float delta2 = (flatKnot.at(polygon.polyP.size()) - flatKnot.at(2 - 1)) / 1000;
		for (float u = flatKnot.at(2 - 1); u < flatKnot.at(polygon.polyP.size()); u += delta2)
			show2(u);
	}
}


