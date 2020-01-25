#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <GL/glut.h>
#include <GL/glu.h>
#include <stdlib.h>    
#include <time.h>
#include <cstdio>
#include "point.h"
#include "curve.h"
#include "util.h"

using namespace std;

const int WINDOW_WIDTH = 640;
const int WINDOW_HEIGHT = 480;
const int WINDOW_SIZE = 600;
const Point Window(WINDOW_WIDTH, WINDOW_HEIGHT);
const int KNOT_WINDOW_WIDTH = 200;
const int KNOT_WINDOW_HEIGHT = 600;
const Point KnotWindow(KNOT_WINDOW_WIDTH, KNOT_WINDOW_HEIGHT);

int window, knotWindow;
int menuId, menuPoint, menuBspline, menu;

void initWindow() {
	glClearColor(0, 0, 0, 0);
	gluOrtho2D(0, 1, 0, 1);
}

void initKnotWindow() {
	glClearColor(0, 0, 0, 0);
	gluOrtho2D(0, 1, 0, 40);
}

void displayFunc() {
	glClear(GL_COLOR_BUFFER_BIT);
	glPointSize(1.0);
	srand(time(NULL));
	glColor3f(rand() % 255 , rand() % 255 , 100);
	glBegin(GL_POINTS);
	for (int i = 0; i < curves.size(); i++)
		curves[i]->show();
	glEnd();
	glPointSize(5.0);
	glBegin(GL_POINTS);
	if (curves.size())
		for (int j = 0; j < curves[curId]->size(); j++) {
			const Point& p = curves[curId]->get(j);
			glVertex2f(p.x, p.y);
		}
	glEnd();
	glPointSize(15.0);
	glColor3f(0, 255, 0);
	glBegin(GL_POINTS);
	if (curves.size() && curves[curId]->size()) {
		const Point& p = curves[curId]->get(curPoint);
		glVertex2f(p.x, p.y);
	}
	if (state == INPUT_STATE) {
		glVertex2f(inputPoint.x, inputPoint.y);
	}
	glEnd();
	glColor3f(255, 255, 255);
	if (curves.size()) {
		for (int j = 0; j < curves[curId]->size(); j++) {
			const Point& p = curves[curId]->get(j);
			glRasterPos2f(p.x - float(4) / float(WINDOW_WIDTH), p.y + float(15) / float(WINDOW_HEIGHT));
			print(GLUT_BITMAP_9_BY_15, j);
		}
		if (curves[curId]->name() == "Bezier" && curves[curId]->size()) {
			glRasterPos2f(0, 0);
			const Point& p = curves[curId]->get(curPoint);
			char buf[50];
			sprintf_s(buf, "Bezier Curve #%d Point #%d=(%.3f, %.3f)", curId, curPoint, p.x, p.y);

			print(GLUT_BITMAP_9_BY_15, buf);
		}
		if (curves[curId]->name() == "Bspline" && curves[curId]->size()) {
			glRasterPos2f(0, 0);
			const Point& p = curves[curId]->get(curPoint);
			char buf[50];
			sprintf_s(buf, "Bspline Curve #%d Point #%d=(%.3f, %.3f) K=%d", curId, curPoint, p.x, p.y, dynamic_cast<Bspline*>(curves[curId])->getK());

			print(GLUT_BITMAP_9_BY_15, buf);
		}
	}
	glutSwapBuffers();
}

void displayKnotFunc() {
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(rand() % 255 , rand() % 255, 1.0);
	if (curves.size() == 0) return;
	if (curves[curId]->name() == "Bspline") {
		Bspline* b = dynamic_cast<Bspline*>(curves[curId]);
		glRasterPos2f(0, 39);
		print(GLUT_BITMAP_9_BY_15, "Number of Knots:");
		for (int i = 0; i < b->knotSize(); i++) {
			if (knotInput && knotPos == i) continue;
			glRasterPos2f(0, 38 - i);
			print(GLUT_BITMAP_9_BY_15, (int) b->getKnot(i));
		}
		if (knotInput) {
			glRasterPos2d(0, 38 - knotPos);
			print(GLUT_BITMAP_9_BY_15, knotString);
			print(GLUT_BITMAP_9_BY_15, "_");
		}
	}
	glutSwapBuffers();
}

void init(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
}

int main(int argc, char** argv) {
	cout << "Welcome!" << endl;
	cout << "Press (q) to create Beizer curve" << endl;
	cout << "Press (w) to create Bspline curve" << endl;
	cout << "Press (z) to select next curve" << endl;
	cout << "Press (d) to delete curve" << endl;
	cout << "Press (n) to select next point" << endl;
	cout << "Press (p) to select previous point" << endl;
	cout << "Press (a) to add point" << endl;
	cout << "Press (r) to remove point" << endl;
	cout << "Press (i) to insert curve" << endl;
	cout << "Press (m) to modify curve" << endl;
	cout << "Press (+) to increment knots to Bspline Curve" << endl;
	cout << "Press (-) to increment knots to Bspline Curve" << endl;
	init(argc, argv);
	glutInitWindowSize(Window.x, Window.y);
	glutInitWindowPosition(100, 100);
	window = glutCreateWindow("OpenGL Project 4");
	initWindow();
	glutDisplayFunc(displayFunc);
	glutKeyboardFunc(keyFunc);
	glutMouseFunc(mouseFunc);
	glutPassiveMotionFunc(motionFunc);
	menuId = glutCreateMenu(IdmenuFunc);
	glutAddMenuEntry("Next Curve(z)", 1);
	glutAddMenuEntry("Prev Curve(x)", 2);
	glutAddMenuEntry("Add Bezier(q)", 3);
	glutAddMenuEntry("Add Bspline(w)", 4);
	glutAddMenuEntry("Remove Current Curve", 5);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
	menuBspline = glutCreateMenu(menuBsplinePointFunc);
	glutAddMenuEntry("Increment K(+)", 1);
	glutAddMenuEntry("Decrement K(-)", 2);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
	menuPoint = glutCreateMenu(pointMenuFunc);
	glutAddMenuEntry("Next Point(n)", 1);
	glutAddMenuEntry("Prev Point(p)", 2);
	glutAddMenuEntry("Add Point(a)", 3);
	glutAddMenuEntry("Remove Current Point(r)", 4);
	glutAddMenuEntry("Insert Point(i)", 5);
	glutAddMenuEntry("Modify Current Point(m)", 6);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
	glutInitWindowSize(KnotWindow.x, KnotWindow.y);
	glutInitWindowPosition(Window.x + 120, 100);
	knotWindow = glutCreateWindow("Knot window");
	initKnotWindow();
	glutDisplayFunc(displayKnotFunc);
	glutMouseFunc(mouseKnotFunc);
	glutKeyboardFunc(keyKnotFunc);

	glutMainLoop();
	return 0;
}
