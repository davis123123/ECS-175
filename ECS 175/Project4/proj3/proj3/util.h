#ifndef UTILS_H
#define UTILS_H

#include <vector>
#include <string>
#include "point.h"
#include "curve.h"

using namespace std;

extern const int WINDOW_WIDTH;
extern const int WINDOW_HEIGHT;
extern int window;
extern int knotWindow;
extern vector<Curve*> curves;
extern int curPoint;
extern void (*inputCallback)(const Point& p);
extern Point inputPoint;
extern int state;
const int INPUT_STATE = 1;
const int NORMAL_STATE = 0;
extern int curId;
extern int knotPos;
extern bool knotInput;
extern string knotString;

void refreshFunc();
void setInputState();
void insertCallback(const Point& p);
void modifyCallback(const Point& p);
void addCallback(const Point& p);
void print(void* format, const char str[]);
void print(void* format, int num);
void motionFunc(int x, int y);
void mouseFunc(int button, int mstate, int x, int y);
void mouseKnotFunc(int button, int mstate, int x, int y);
void pointMenuFunc(int menu);
void IdmenuFunc(int menu);
void menuBsplinePointFunc(int menu);
void keyFunc(unsigned char ch, int x, int y);
void keyKnotFunc(unsigned char ch, int x, int y);
void print(void* format, const string& str);
void print(void* format, float num);
float getFloat(const string& str);
void save(const vector<Curve*>& curves);
#endif
