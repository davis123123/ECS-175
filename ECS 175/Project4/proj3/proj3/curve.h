#ifndef CURVE_H
#define CURVE_H
#include "polygon.h"
#include "point.h"
#include <vector>
#include <string>

using namespace std;

class Curve {
protected:
	POLYGON polygon;
public:
	int size() const;
	virtual void set(int pos, const Point& p);
	virtual void add(const Point& p);
	virtual void remove(int pos);
	const Point& get(int pos) const;
	virtual void show() const = 0;
	virtual void insert(int pos, const Point& p);
	virtual const string name() const = 0;
	
};

class Bezier : public Curve {
private:
	void show(float t) const;
	void dda(double, double, double, double) const;
	void show2(float u) const;
public:
	void show() const;
	const string name() const {
		return "Bezier";
	}
};

class Bspline : public Curve {
protected:
	vector<float> knots;
	int k;
private:
	void addKnot();
	void show(float u) const;
	void show2(float u) const;
public:
	Bspline() :Curve(), k(0) { setK(2); }
	int knotSize() const;
	void add(const Point& p);
	void remove(int pos);
	void setKnot(int pos, float k);
	float getKnot(int pos) const;
	void insert(int pos, const Point& p);
	void setK(int k);
	void addK(int dk);
	int getK() const;
	const string name() const {
		return "Bspline";
	}
public:
	void show() const;
};

#endif
