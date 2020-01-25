#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <ctime>
#include <iostream>
#include <sstream>
#include <vector>
#include "gl.h"
#include "object.h"

using namespace std;

const Point2i Window(600, 600);
int window;

float canvas[600 * 600 * 3];

vector<Prim*> objects;

Point from(0, -3, 0);
Point at(0, 0, 0);
Point up(0, 0, 1);
Point rv;
float angDeg = 45;
int resolution = 600;
int depth = 5;

Line ray(int x, int y) {
	Line ret;
	ret.start = from;
	float xx = (float)x / (float)resolution * 2 - 1;
	float yy = (float)y / (float)resolution * 2 - 1;
	Point p = from;
	p += tan(angDeg / 180 * PI) * xx * rv;
	p += tan(angDeg / 180 * PI) * yy * up;
	p += (at - from).norm();
	ret.u = (p - from).norm();
	return ret;
}

void updateVector() {
	up.norm();
	rv = cross(at - from, up).norm();
}

void initWindow() {
	glClearColor(0, 0, 0, 0);
}

void display() {
	glClear(GL_COLOR_BUFFER_BIT);
	memset(canvas, 0.0f, sizeof(canvas));
	for (int x = 0; x < resolution; x++)
		for (int y = 0; y < resolution; y++) {
			Line r = ray(x, y);
			Color f = light(r, objects, depth);
			makePixel(x, y, f, canvas, resolution);
		}
	normalize(canvas, resolution);
	glDrawPixels(resolution, resolution, GL_RGB, GL_FLOAT, canvas);
	glutSwapBuffers();
}

void refreshFunc() {
	glutPostWindowRedisplay(window);
}

void get() {
	cout << "from=" << from << endl;
	cout << "at=" << at << endl;
	cout << "up=" << up << endl;
	cout << "angle=" << angDeg << endl;
	cout << "I_a=" << iA << " I_l=" << iL << endl;
	cout << "k_a=" << kA << " k_d=" << kD << " k_s=" << kS << endl;
	cout << "K=" << bigK << " n=" << bigN << endl;
	cout << "k_r=" << kR << " k_t=" << kT << endl;
	cout << "eta=" << eta << endl;
	cout << "depth=" << depth << endl;
	cout << "resolution=" << resolution << endl;
}

void set(const string& command) {
	if (!command.size()) return;
	int pos = command.find('=');
	string name = command.substr(0, pos);
	stringstream value(command.substr(pos + 1, command.size() - pos - 1));
	cout << name << " " << value.str() << endl;
	if (name == "from") value >> from;
	if (name == "at") value >> at;
	if (name == "up") value >> up;
	if (name == "angle") value >> angDeg;
	if (name == "I_a") value >> iA;
	if (name == "k_d") value >> kD;
	if (name == "k_t") value >> kT;
	if (name == "k_s") value >> kS;
	if (name == "K") value >> bigK;
	if (name == "n") value >> bigN;
	if (name == "k_r") value >> kR;
	if (name == "I_l") value >> iL;
	if (name == "k_a") value >> kA;
	if (name == "eta") value >> eta;
	if (name == "depth") value >> depth;
	if (name == "resolution") value >> resolution;
}

void set() {
	string command;
	cout << ">";
	getline(cin, command);
	set(command);
	updateVector();
	cout << "Ok" << endl;
	refreshFunc();
}


void init(int argc, char** argv) {
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(Window.x, Window.y);
	glutInitWindowPosition(100, 100);

	Quad* s1 = newSphere(Point(-1.5, 1, -0.5), 0.4);
	s1->color = Color(1, 1, 0);
	s1->transparent = false;
	Quad* s2 = newSphere(Point(1, 3, 1.5), 0.4);
	s2->color = Color(1, 1.5, 1);
	s2->transparent = true;
	Poly* t1 = newTriangle(Point(-1, 2, 2),
		Point(-1, 2, 1),
		Point(-1, 1, 2),
		Point(-1, 0, 0));
	t1->color = Color(1, 1, 1);
	t1->transparent = true;
	Poly* t2 = newTriangle(Point(2, 3, 1),
		Point(2, 3, 1),
		Point(0, 3, 2),
		Point(0, 1, 0));
	t2->color = Color(1, 2.5, 1);
	t2->transparent = true;
	Poly* t3 = newTriangle(Point(0, 2, 2),
		Point(-1, 1, 2),
		Point(-1, 2, 1),
		Point(1, -1, -1));
	t3->color = Color(1, 1.5, 2.25);
	t3->transparent = true;
	Poly* p1 = newSquare(Point(5, 6, -4), Point(5, 6, 4),
		Point(-5, 6, 4), Point(-5, 6, -4),
		Point(0, -1, 0));
	p1->color = Color(0.5, 0.5, 1);
	p1->transparent = false;
	Poly* p2 = newSquare(Point(-5, -5, -3), Point(-5, 5, -3),
		Point(-5, 5, 3), Point(-5, -5, 3),
		Point(1, 0, 0));
	p2->color = Color(1, 0.5, 0);
	p2->transparent = false;
	Poly* p3 = newSquare(Point(3, 4, -2), Point(3, -4, -2),
		Point(3, -4, 2), Point(3, 4, 2),
		Point(-1, 0, 0));
	p3->color = Color(0.2, 1, 0);
	p3->transparent = false;
	objects.push_back(s1);
	objects.push_back(s2);
	objects.push_back(t1);
	objects.push_back(t2);
	objects.push_back(t3);
	objects.push_back(p1);
	objects.push_back(p2);
	objects.push_back(p3);
	updateVector();
}

void keyFunc(unsigned char ch, int x, int y) {
	if (ch == 'g')
		get();
	if (ch == 'c')
		set();
}

int main(int argc, char** argv) {
	init(argc, argv);
	window = glutCreateWindow("Project 5");
	initWindow();
	glutDisplayFunc(display);
	glutKeyboardFunc(keyFunc);
	glutMainLoop();
	return 0;
}
