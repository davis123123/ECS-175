#include "util.h"
#include <typeinfo>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <cstdio>
#include <sstream>
#include <GL/glut.h>
#define _CRT_SECURE_NO_WARNINGS
using namespace std;

int state = NORMAL_STATE;
vector<Curve*> curves;
int curId = 0;
int curPoint = 0;
void (*inputCallback)(const Point& p);
Point inputPoint;
bool knotInput = false;
int knotPos = false;
string knotString;

void refreshFunc() {
	glutPostWindowRedisplay(window);
	glutPostWindowRedisplay(knotWindow);
}

void setInputState() {
	state = INPUT_STATE;
}

void insertCallback(const Point& p) {
	if (curves.size() == 0)
		throw runtime_error("Create a type of curve first!!");
	curves[curId]->insert(curPoint, p);
}

void modifyCallback(const Point& p) {
	if (curves.size() == 0)
		throw runtime_error("Create a type of curve first!!");
	curves[curId]->set(curPoint, p);
}

void addCallback(const Point& p) {
	if(curves.size() == 0)
		throw runtime_error("Create a type of curve first!!");
	curves[curId]->add(p);
}

void mouseFunc(int button, int mstate, int x, int y) {
	if (button == GLUT_LEFT_BUTTON && mstate == GLUT_DOWN && state == INPUT_STATE) {
		inputCallback(inputPoint);
		state = NORMAL_STATE;
	}
	refreshFunc();
}

void mouseKnotFunc(int button, int mstate, int x, int y) {
	if (button == GLUT_LEFT_BUTTON && mstate == GLUT_DOWN) {
		if (curves.size() != 0 && curves[curId]->name() == "Bspline") {
			Bspline* b = dynamic_cast<Bspline*>(curves[curId]);
			knotPos = y / 15 - 1;
			if (0 <= knotPos && knotPos < b->knotSize()) {
				knotInput = true;
				knotString.clear();
			}
		}
	}
	refreshFunc();
}

void motionFunc(int x, int y) {
	inputPoint.x = (float) x / WINDOW_WIDTH;
	inputPoint.y = 1 - (float)y / WINDOW_HEIGHT;
	if (state == INPUT_STATE && inputCallback == modifyCallback)
		inputCallback(inputPoint);
	refreshFunc();
}

void pointMenuFunc(int menu) {
	switch (menu) {
	case 1:
		if (curves[curId]->size()) {
			curPoint += 1;
			curPoint %= curves[curId]->size();
		}
		break;
	case 2:
		if (curves[curId]->size()) {
			curPoint += curves[curId]->size() - 1;
			curPoint %= curves.size();
		}
		break;
	case 3:
		inputCallback = addCallback;
		setInputState();
		break;
	case 4:
		if (curves[curId]->size()) {
			curves[curId]->remove(curPoint);
		}
		break;
	case 5:
		inputCallback = insertCallback;
		setInputState();
		break;
	case 6:
		inputCallback = modifyCallback;
		setInputState();
		break;
	}
	save(curves);
	refreshFunc();
}

void IdmenuFunc(int menu) {
	switch (menu) {
	case 1:
		curId += 1;
		curId %= curves.size();
		curPoint = 0;
		break;
	case 2:
		curId += curves.size() - 1;
		curId %= curves.size();
		curPoint = 0;
		break;
	case 3:
		curves.push_back(new Bezier());
		break;
	case 4:
		curves.push_back(new Bspline());
		break;
	case 5:
		Curve * curve = curves[curId];
		curves.erase(curves.begin() + curId);
		delete curve;
		break;
	}
	save(curves);
	refreshFunc();
}

void menuBsplinePointFunc(int menu) {
	switch (menu) {
	case 1:
		if (curves[curId]->name() == "Bspline")
			dynamic_cast<Bspline*>(curves[curId])->addK(1);
		break;
	case 2:
		if (curves[curId]->name() == "Bspline")
			dynamic_cast<Bspline*>(curves[curId])->addK(-1);
		break;
	}
	save(curves);
	refreshFunc();
}

void keyFunc(unsigned char ch, int x, int y) {
	switch (ch) {
	case 'N': case 'n': pointMenuFunc(1); break;
	case 'P': case 'p': pointMenuFunc(2); break;
	case 'A': case 'a': pointMenuFunc(3); break;
	case 'R': case 'r': pointMenuFunc(4); break;
	case 'I': case 'i': pointMenuFunc(5); break;
	case 'M': case 'm': pointMenuFunc(6); break;
	case 'Z': case 'z': IdmenuFunc(1); break;
	case 'X': case 'x': IdmenuFunc(2); break;
	case 'Q': case 'q': IdmenuFunc(3); break;
	case 'W': case 'w': IdmenuFunc(4); break;
	case 'D': case 'd': IdmenuFunc(5); break;
	case '=': case '+': menuBsplinePointFunc(1); break;
	case '_': case '-': menuBsplinePointFunc(2); break;
	}
	refreshFunc();
}

void keyKnotFunc(unsigned char ch, int x, int y) {
	switch (ch) {
	case '1': case '2': case '3': case '4':
	case '5': case '6': case '7': case '8':
	case '9': case '0': case '.':
		if (knotInput) knotString += ch;
		break;
	case '\r':
		if (knotInput) knotInput = false;

		if (curves[curId]->name() == "Bspline")
			dynamic_cast<Bspline*>(curves[curId])->setKnot(knotPos, getFloat(knotString));
		break;
	case '\b':
		if (knotInput && knotString.size()) knotString.erase(knotString.size() - 1);
		break;
	case '\033':
		if (knotInput) knotInput = false;
	}
	save(curves);
	refreshFunc();
}

void print(void* format, const string& str) {
	for (int i = 0; i < str.size(); i++)
		glutBitmapCharacter(format, str[i]);
}

void print(void* format, const char str[]) {
	while (*str) glutBitmapCharacter(format, *str++);
}

void print(void* format, int num) {
	char buf[10];
	sprintf_s(buf, "%d", num);
	print(format, string(buf));
}

void print(void* format, float num) {
	char buf[10];
	sprintf_s(buf, "%.3f", num);
	print(format, string(buf));
}

float getFloat(const string& str) {
	float ret;
	stringstream tokenizer;
	tokenizer << str;
	tokenizer >> ret;
	return ret;
}


void save(const vector<Curve*>& curves) {
	ofstream out("data.txt");
	out << setiosflags(ios::fixed) << setprecision(3);
	out << curves.size() << endl << endl;
	for (int i = 0; i < curves.size(); i++) {
		if (curves[i]->name() == "Bspline") {
			Bspline* c = dynamic_cast<Bspline*>(curves[i]);
			out << c->size() << ' ' << c->getK() << endl;
			for (int i = 0; i < c->size(); i++) { out << c->get(i).x << ' ' << c->get(i).y << endl; }
			for (int i = 0; i < c->size() + c->getK(); i++) { out << c->getKnot(i) << ' '; }
			out << endl << endl;
		}
		else {
			Curve* c = curves[i];
			out << c->size() << ' ' << 0 << endl;
			for (int i = 0; i < c->size(); i++) { out << c->get(i).x << ' ' << c->get(i).y << endl; }
			out << endl;
		}
	}
}
