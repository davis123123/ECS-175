#include <windows.h>
#include <GL/gl.h>
#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <map>
#include <vector>
#include <fstream>
#include <sstream>
#include <set> 
#include <string>
#include <thread> 
#define SIN(x) sin(x * 3.141592653589/180)
#define COS(x) cos(x * 3.141592653589/180)

using namespace std;

struct PIXEL {
	int red;
	int green;
	int blue;
};

struct PNT {
	double x;
	double y;
	double z;
	double normX = 0;
	double normY = 0;
	double normZ = 0;
	PIXEL pixel;
	vector<int> connFace;
};
struct POLYGON {
	vector<PNT> polyP;
	vector<PIXEL> polyC;
};
struct JOIN {
	int start;
	//PIXEL startPix;
	int mid;
	//PIXEL midPix;
	int end;
	//PIXEL endPix;
	double spec;
	map<int, PNT> sm;
	map<int, PNT> me;
	map<int, PNT> se;
	PNT normVec;
};

struct JOINP {
	vector<JOIN> joinP;
};

typedef double Matrix4x4[4][4];
Matrix4x4 matRot;

void rasG();
void reshape(int, int);
void command();
void setframe(int, char**);
void init();
void display();
void drawXY(POLYGON, int);
void drawXZ(POLYGON, int);
void drawYZ(POLYGON, int);
PNT matrixPreMultiply(Matrix4x4, PNT);
void matrix4x4PreMultiply(Matrix4x4, Matrix4x4);
void matrix4x4SetIdentity(Matrix4x4);
POLYGON translate3D(POLYGON, double, double, double);
void rotTranslate3D(double, double, double);
void rotate3D(POLYGON, PNT, PNT, int);
void dda(double, double, double, double);
map<int, PNT> ddaG(double, double, double, double, PIXEL, PIXEL);
PNT centre(POLYGON);
POLYGON scale(POLYGON, double, double);
void initPinkLine();
vector<POLYGON> boundingBox(vector<POLYGON>);
vector<POLYGON> boundingBox2(vector<POLYGON>);
vector<POLYGON> boundingBox3(vector<POLYGON>);
PNT getMax(vector<POLYGON>);
PNT getMin(vector<POLYGON>);
void drawBox();
void drawAll(vector<POLYGON>);
void writeFile();
vector<POLYGON> painterAlgX(vector<POLYGON>);
vector<POLYGON> painterAlgY(vector<POLYGON>);
vector<POLYGON> painterAlgZ(vector<POLYGON>);
PNT getMaxPol(POLYGON polygon);
void phongShade(PIXEL, double, double, PNT, PIXEL, PNT, POLYGON);
void rasterizePhong(JOIN, float, float, float, float, float, POLYGON);
void rasterization(JOIN, POLYGON);
POLYGON normalize(POLYGON, JOINP);
void gouraudShade();
void gouraudShadeY();
void gouraudShadeZ();
int orientation(PNT, PNT, PNT);
bool onSegment(PNT, PNT, PNT);
int isInside(JOIN, int, PNT, POLYGON);
int checkBetweenLine(JOIN, PNT, POLYGON);
bool doIntersect(PNT, PNT, PNT, PNT);
PIXEL interpolate(PIXEL, PIXEL, float, float, float);
double mag(PNT, PNT);
PNT lightvec(PNT, PNT);
PNT reflectVec(PNT, PNT);
double dotP(PNT, PNT);
PNT normP(JOIN, POLYGON);
int ac, n, polyN;
char** av;
vector<POLYGON> polygonL;

int grid_width = 640;
int grid_height = 480;
float pixel_size = 1;
int win_height = grid_height * pixel_size;
int win_width = grid_width * pixel_size;
int polyNum;
vector<JOINP> joinL;
PNT p1, p2;
bool bb = false;
map<int, PNT> vertexFace;
PIXEL light, am;
PNT f, l;
int main(int argc, char* argv[]) {

	ifstream infile("bunny.txt");

	string line;
	getline(infile, line);
	istringstream iss(line);
	iss >> polyNum;

	int edgeNum, joinNum;
	double xp, yp, zp;

	for (int h = 0; h < polyNum; h++) {
		getline(infile, line);
		while (line.length() == 0)
			getline(infile, line);
		istringstream isss(line);
		isss >> edgeNum;
		POLYGON poly;
		for (int i = 0; i < edgeNum; i++) {
			getline(infile, line);
			istringstream iss(line);
			iss >> xp >> yp >> zp;
			PNT p;
			p.x = xp;
			p.y = yp;
			p.z = zp;
			poly.polyP.push_back(p);
		}

		for (int i = 0; i < edgeNum; i++) {
			getline(infile, line);
			istringstream iss(line);
			iss >> xp >> yp >> zp;
			poly.polyP[i].pixel.red = xp;
			poly.polyP[i].pixel.green = yp;
			int t = zp;
			poly.polyP[i].pixel.blue = t;
		}

		polygonL.push_back(poly);
		
		getline(infile, line);
		istringstream issss(line);
		issss >> joinNum;
		JOINP joinP;
		for (int i = 0; i < joinNum; i++) {
			JOIN j;
			int js, je, jm;
			getline(infile, line);
			istringstream iss(line);
			iss >> js >> jm >>je;
			j.start = js;
			j.mid = jm;
			j.end = je;
			joinP.joinP.push_back(j);
		}
		for (int i = 0; i < joinNum; i++) {
			int sp;
			getline(infile, line);
			istringstream iss(line);
			iss >> sp;
			joinP.joinP[i].spec = sp;
		}

		joinL.push_back(joinP);
	}

	
	polygonL[0] = scale(polygonL[0], 2000, polygonL[0].polyP.size());
	polygonL[0] = translate3D(polygonL[0], 280, 200, 0);
	ac = argc;
	av = argv;
	string yn;
	setframe(argc, argv);
	init();
	return 0;
}

void setframe(int c, char** v) {
	glutInit(&c, v);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowPosition(1, 1);
	glutInitWindowSize(grid_width, grid_height);
	glutCreateWindow("glut demo");
}

void init() {
	glMatrixMode(GL_PROJECTION); //Set projection parameters.
	glLoadIdentity(); //ultiply current matrix by identity matrix (for BRESSEN)
	gluOrtho2D(0.0, 640.0, 0.0, 480);
	glutDisplayFunc(display);

	glutIdleFunc(command);
	glutMainLoop();

}

void command() {
	//cout << "0 to display" << endl;
	cout << "1 to display Phong Model on Vertex" << endl;
	cout << "2 for Gouraud Shading" << endl;
	cout << "3 for Scale " << endl;
	cout << "4 for Translate" << endl;
	cout << "Enter Number:" << endl;
	cin >> n;
	glutPostRedisplay();

}

void display() {
	glClear(GL_COLOR_BUFFER_BIT);
	glClearColor(0.0, 0.0, 0.0, 0.0); //Set display WIndow color to white

	switch (n) {
	case 0: {
		//drawAll(polygonL);
		
	}break;
	case 1: {//Phong
		cout << "Select ambient colour of light source(R G B):" << endl;
		float ar, ab, ag;
		cin >> ar >> ag >> ab;
		cout << "Enter intensity of light source:" << endl;
		float li;
		cin >> li;
		cout << "Enter distance of light source:" << endl;
		float k;
		cin >> k;
		cout << "Enter color of light source(R G B):" << endl;
		float lb, lr, lg;
		cin >> lr >> lg >> lb;
		light.blue = lb;
		light.red = lr;
		light.green = lg;
		am.blue = ab;
		am.red = ar;
		am.green = ag;
		cout << "Enter location of light source (x y z):" << endl;
		float lx, ly, lz;
		cin >> lx >> ly >> lz;
		PNT f, l;
		f.x = 2;
		f.y = 2;
		f.z = 2;
		l.x = lx;
		l.y = ly;
		l.z = lz;
		for (int i = 0; i < polygonL.size(); i++) {
			phongShade(am, li, k, f, light, l, polygonL[i]);
		}
		gouraudShade();
		gouraudShadeY();
		gouraudShadeZ();
		writeFile();
	} break;
	case 2: {//Gourand
		rasG();
		writeFile();
	}break;
	case 3: {//Halftone
		double ssSize;
		cout << "Select POLYGON Number to scale:" << endl;
		cin >> polyN;
		cout << "Enter Scale Factor:" << endl;
		cin >> ssSize;
		polygonL[polyN] = scale(polygonL[polyN], ssSize, polygonL[polyN].polyP.size());
		//polygonL = boundingBox(polygonL);
		//drawAll(polygonL);
		writeFile();
	}break;
	case 4: {//Painter's
		int ix, iy, iz;
		cout << "Select POLYGON Number to translate:" << endl;
		cin >> polyN;
		cout << "Enter vector x value:" << endl;
		cin >> ix;
		cout << "Enter vector y value:" << endl;
		cin >> iy;
		cout << "Enter vector z value:" << endl;
		cin >> iz;
		polygonL[polyN] = translate3D(polygonL[polyN], ix, iy, iz);
		/*for(int i = 0; i < polygonL.size(); i++){
		  drawAll(polygonL[i], i);
		}*/
		//drawAll(polygonL);
		writeFile();
	}break;
	case 5: {//Halftone

		glutReshapeWindow(grid_width, grid_height);
		glFlush();
		writeFile();
	}break;
	default:
		break;
	}
	//writeFile();
}

void writeFile() {
	ofstream myfile;
	myfile.open("bunny");
	myfile << polyNum << endl;
	for (int i = 0; i < polyNum; i++) {
		myfile << polygonL[i].polyP.size() << endl;

		for (int j = 0; j < polygonL[i].polyP.size(); j++) {
			myfile << polygonL[i].polyP[j].x << " " << polygonL[i].polyP[j].y << " " << polygonL[i].polyP[j].z << endl;
		}

		myfile << joinL[i].joinP.size() << endl;
		for (int j = 0; j < joinL[i].joinP.size(); j++) {
			myfile << joinL[i].joinP[j].start << " " << joinL[i].joinP[j].mid << joinL[i].joinP[j].end <<endl;
		}
		for (int j = 0; j < joinL[i].joinP.size(); j++) {
			myfile << joinL[i].joinP[j].spec << endl;
		}
	}

}


void drawAll(vector<POLYGON> polygonL) {
	for (unsigned int i = 0; i < polygonL.size(); i++) {
		vector<POLYGON> temp = polygonL;

		glClear(GL_COLOR_BUFFER_BIT);
		glClearColor(1.0, 1.0, 1.0, 0.0);
		glEnable(GL_SCISSOR_TEST);
		//if (bb)
			//temp = boundingBox(polygonL);
		
		drawXY(polygonL[i], i);
		glDisable(GL_SCISSOR_TEST);

		glEnable(GL_SCISSOR_TEST);
		if (bb)
			temp = boundingBox2(polygonL);
		drawYZ(temp[i], i);
		glDisable(GL_SCISSOR_TEST);

		glEnable(GL_SCISSOR_TEST);
		if (bb)
			temp = boundingBox3(polygonL);
		drawXZ(temp[i], i);
		glDisable(GL_SCISSOR_TEST);
	}
	glFlush();
}

vector<POLYGON> boundingBox(vector<POLYGON> polygon) {
	PNT maxP, minP;

	maxP = getMax(polygon);
	minP = getMin(polygon);
	POLYGON centrePoly;
	centrePoly.polyP.push_back(minP);
	centrePoly.polyP.push_back(maxP);
	PNT centreP = centre(centrePoly);

	double xdiff = (centreP.x - (grid_width / 2)) * -1,
		ydiff = (centreP.y - (grid_height / 2)) * -1,
		zdiff = (centreP.z - (grid_height / 2)) * -1;
	//translate
	for (unsigned int i = 0; i < polygon.size(); i++) {
		polygon[i] = translate3D(polygon[i], xdiff, ydiff, 0);
	}

	maxP = getMax(polygon);
	minP = getMin(polygon);
	//Scale
	//cout << "scale 3  " << maxP.x << " " << maxP.y << endl;
	if (maxP.x < grid_width * 4 / 5 || maxP.y < grid_height * 4 / 5) {
		while (maxP.x < grid_width * 4 / 5 && maxP.y < grid_height * 4 / 5) {
			for (unsigned int i = 0; i < polygon.size(); i++) {
				polygon[i] = scale(polygon[i], 1.1, polygon[i].polyP.size());
			}
			maxP = getMax(polygon);
			minP = getMin(polygon);
		}
	}

	//cout <<maxP.x<<"scale 2"<<minP.x<<endl;
	while ((maxP.x >= grid_width * 9 / 10 || maxP.y >= grid_height * 9 / 10) || (minP.x < 0 || minP.y < 0)) {
		for (unsigned int i = 0; i < polygon.size(); i++) {
			polygon[i] = scale(polygon[i], 0.9, polygon[i].polyP.size());
		}
		//cout <<maxP.x<<"scale 2"<<minP.x<<endl;
		maxP = getMax(polygon);
		minP = getMin(polygon);
	}

	return polygon;
}

vector<POLYGON> boundingBox2(vector<POLYGON> polygon) {
	PNT maxP, minP;

	maxP = getMax(polygon);
	minP = getMin(polygon);
	POLYGON centrePoly;
	centrePoly.polyP.push_back(minP);
	centrePoly.polyP.push_back(maxP);
	PNT centreP = centre(centrePoly);

	double ydiff = (centreP.y - (grid_width / 2)) * -1,
		zdiff = (centreP.z - (grid_height / 2)) * -1;
	//translate
	for (unsigned int i = 0; i < polygon.size(); i++) {
		polygon[i] = translate3D(polygon[i], 0, ydiff, zdiff);
	}

	maxP = getMax(polygon);
	minP = getMin(polygon);
	//  cout <<"scale 11  "<< minP.y<<" "<<minP.z<<"  "<<
	//  maxP.y<<" "<<maxP.z<<endl;
	if (maxP.y < grid_width * 4 / 5 || maxP.z < grid_height * 4 / 5) {
		while (maxP.y < grid_width * 4 / 5 && maxP.z < grid_height * 4 / 5) {
			for (unsigned int i = 0; i < polygon.size(); i++) {
				polygon[i] = scale(polygon[i], 1.01, polygon[i].polyP.size());
			}

			maxP = getMax(polygon);
			minP = getMin(polygon);
		}
	}
	//cout <<maxP.x<<"scale 2"<<minP.x<<endl;
	while ((maxP.y >= grid_width * 9 / 10 || maxP.z >= grid_height * 9 / 10) ||
		(minP.y < 0 || minP.z < 0)) {
		for (unsigned int i = 0; i < polygon.size(); i++) {
			polygon[i] = scale(polygon[i], 0.9, polygon[i].polyP.size());
		}
		//cout <<maxP.x<<"scale 2"<<minP.x<<endl;
		maxP = getMax(polygon);
		minP = getMin(polygon);
	}

	return polygon;
}

vector<POLYGON> boundingBox3(vector<POLYGON> polygon) {
	PNT maxP, minP;

	maxP = getMax(polygon);
	minP = getMin(polygon);
	POLYGON centrePoly;
	centrePoly.polyP.push_back(minP);
	centrePoly.polyP.push_back(maxP);
	PNT centreP = centre(centrePoly);

	double xdiff = (centreP.x - (grid_width / 2)) * -1,
		zdiff = (centreP.z - (grid_height / 2)) * -1;
	//translate
	for (unsigned int i = 0; i < polygon.size(); i++) {
		polygon[i] = translate3D(polygon[i], xdiff, 0, zdiff);
	}

	maxP = getMax(polygon);
	minP = getMin(polygon);
	//Scale

	if (maxP.x < grid_width * 4 / 5 || maxP.z < grid_height * 4 / 5) {
		while (maxP.x < grid_width * 4 / 5 && maxP.z < grid_height * 4 / 5) {
			for (unsigned int i = 0; i < polygon.size(); i++) {
				polygon[i] = scale(polygon[i], 1.01, polygon[i].polyP.size());
			}
			//  cout <<"scale 1  "<< minP.x<<" "<<minP.y<<endl;
			maxP = getMax(polygon);
			minP = getMin(polygon);
		}
	}
	//cout <<maxP.x<<"scale 2"<<minP.x<<endl;
	while ((maxP.x >= grid_width * 9 / 10 || maxP.z >= grid_height * 9 / 10) ||
		(minP.x < 0 || minP.z < 0)) {
		for (unsigned int i = 0; i < polygon.size(); i++) {
			polygon[i] = scale(polygon[i], 0.9, polygon[i].polyP.size());
		}
		//cout <<maxP.x<<"scale 2"<<minP.x<<endl;
		maxP = getMax(polygon);
		minP = getMin(polygon);
	}

	return polygon;
}

void drawBox() {
	dda(1, 1, 1, grid_height - 1);
	dda(grid_width - 1, grid_width - 1, 1, grid_height - 1);
	dda(1, grid_width - 1, 1, 1);
	dda(1, grid_width - 1, grid_height - 1, grid_height - 1);
}

PNT getMax(vector<POLYGON> polygon) {
	double xMax = 0, yMax = 0, zMax = 0;
	for (unsigned int i = 0; i < polygon.size(); i++) {
		for (unsigned int j = 0; j < polygon[i].polyP.size(); j++) {
			xMax = max(xMax, polygon[i].polyP[j].x);
			yMax = max(yMax, polygon[i].polyP[j].y);
			zMax = max(zMax, polygon[i].polyP[j].z);

		}
	}
	PNT p;
	p.x = xMax;
	p.y = yMax;
	p.z = zMax;
	return p;
}

PNT getMin(vector<POLYGON> polygon) {
	double xMin = 100000, yMin = 100000, zMin = 100000;
	for (unsigned int i = 0; i < polygon.size(); i++) {
		for (unsigned int j = 0; j < polygon[i].polyP.size(); j++) {
			xMin = min(xMin, polygon[i].polyP[j].x);
			yMin = min(yMin, polygon[i].polyP[j].y);
			zMin = min(zMin, polygon[i].polyP[j].z);
		}
	}
	PNT p;
	p.x = xMin;
	p.y = yMin;
	p.z = zMin;
	return p;
}

PNT getMaxPol(POLYGON polygon) {
	double xMax = 0, yMax = 0, zMax = 0;

	for (unsigned int j = 0; j < polygon.polyP.size(); j++) {
		xMax = max(xMax, polygon.polyP[j].x);
		yMax = max(yMax, polygon.polyP[j].y);
		zMax = max(zMax, polygon.polyP[j].z);

	}
	
	PNT p;
	p.x = xMax;
	p.y = yMax;
	p.z = zMax;
	return p;
}

vector<POLYGON> painterAlgZ(vector<POLYGON> polygonL) {
	for (unsigned int i = 0; i < polygonL.size(); i++) {//sort by z value
		for (unsigned int j = 0; j < polygonL.size() - i; j++) {
			if (j == i) continue;
			if (getMaxPol(polygonL[j]).z > getMaxPol(polygonL[j + 1]).z) {
				POLYGON temp = polygonL[j];
				JOINP tempj = joinL[j];
				joinL[j] = joinL[j + 1];
				polygonL[j] = polygonL[j + 1];
				joinL[j + 1] = tempj;
				polygonL[j + 1] = temp;
			}
		}
	}
	return polygonL;
}//add joinL

vector<POLYGON> painterAlgX(vector<POLYGON> polygonL) {
	for (unsigned int i = 0; i < polygonL.size(); i++) {//sort by z value
		for (unsigned int j = 0; j < polygonL.size() - i; j++) {
			if (j == i) continue;
			if (getMaxPol(polygonL[j]).x > getMaxPol(polygonL[j + 1]).x) {
				POLYGON temp = polygonL[j];
				JOINP tempj = joinL[j];
				joinL[j] = joinL[j + 1];
				polygonL[j] = polygonL[j + 1];
				joinL[j + 1] = tempj;
				polygonL[j + 1] = temp;
			}
		}
	}
	return polygonL;
}

vector<POLYGON> painterAlgY(vector<POLYGON> polygonL) {
	for (unsigned int i = 0; i < polygonL.size(); i++) {//sort by z value
		
		for (unsigned int j = 0; j < polygonL.size() - i; j++) {
			if (j == i) continue;
			if (getMaxPol(polygonL[j]).y > getMaxPol(polygonL[j + 1]).y) {
				POLYGON temp = polygonL[j];
				JOINP tempj = joinL[j];
				joinL[j] = joinL[j + 1];
				polygonL[j] = polygonL[j + 1];
				joinL[j + 1] = tempj;
				polygonL[j + 1] = temp;
			}
		}
	}
	return polygonL;
}


POLYGON normalize(POLYGON polygon, JOINP join) {
	map<int, POLYGON> normVert;
	for (unsigned int i = 0; i < joinL.size(); i++) {
		for (unsigned int j = 0; j < joinL[i].joinP.size(); j++) {
			joinL[i].joinP[j].normVec = normP(joinL[i].joinP[j],polygonL[i]);
			polygonL[i].polyP[joinL[i].joinP[j].start - 1].connFace.push_back(j);
			polygonL[i].polyP[joinL[i].joinP[j].end - 1].connFace.push_back(j);
			polygonL[i].polyP[joinL[i].joinP[j].mid - 1].connFace.push_back(j);
		}//get facenormal and push into point which list of faces connected to it
	}
	
	for (unsigned int i = 0; i < joinL.size(); i++) {
		for (unsigned int j = 0; j < polygonL[i].polyP.size(); j++) {
			for (int k = 0; k < polygonL[i].polyP[j].connFace.size(); k++) {
				polygonL[i].polyP[j].normX += 
					joinL[i].joinP[polygonL[i].polyP[j].connFace[k]].normVec.x;
				polygonL[i].polyP[j].normY +=
					joinL[i].joinP[polygonL[i].polyP[j].connFace[k]].normVec.y;
				polygonL[i].polyP[j].normZ +=
					joinL[i].joinP[polygonL[i].polyP[j].connFace[k]].normVec.z;
			}
			polygonL[i].polyP[j].normX /= polygonL[i].polyP[j].connFace.size();
			polygonL[i].polyP[j].normY /= polygonL[i].polyP[j].connFace.size();
			polygonL[i].polyP[j].normZ /= polygonL[i].polyP[j].connFace.size();
			double magg = sqrt(polygonL[i].polyP[j].normX * polygonL[i].polyP[j].normX +
				polygonL[i].polyP[j].normY * polygonL[i].polyP[j].normY +
				polygonL[i].polyP[j].normZ * polygonL[i].polyP[j].normZ);
			
			polygonL[i].polyP[j].normX /= magg;
			polygonL[i].polyP[j].normY /= magg;
			polygonL[i].polyP[j].normZ /= magg;
		}
	}//get avergae normal vec
	
	return polygon;
}

void rasG() {
	glEnable(GL_SCISSOR_TEST);
	glViewport(0, 0, grid_width / 2, grid_height / 2);
	glScissor(0, 0, grid_width / 2, grid_height / 2);
	for (unsigned int i = 0; i < joinL.size(); i++) {
		for (unsigned int j = 0; j < joinL[i].joinP.size(); j++) {
			//thread th1(rasterization, joinL[i].joinP[j], polygonL[i]);
			rasterization(joinL[i].joinP[j], polygonL[i]);
			//	cout << "here" << endl;
		}
	}
	glDisable(GL_SCISSOR_TEST);
}

void gouraudShade() {

	glEnable(GL_SCISSOR_TEST);
	glViewport(0, 0, grid_width / 2, grid_height / 2);
	glScissor(0, 0, grid_width / 2, grid_height / 2);
	drawBox();
	
	POLYGON tempPoly = normalize(polygonL[0], joinL[0]);
		//ambience, light intensity, distance, eye point, light colour, light location
	
	
	for (unsigned int i = 0; i < joinL.size(); i++) {
		//polygonL[i] = normalize(polygonL[i], joinL[i]);
		vector<POLYGON> polygonS = painterAlgZ(polygonL);
		for (unsigned int j = 0; j < joinL[i].joinP.size(); j++) {
			double xStart = polygonS[i].polyP[joinL[i].joinP[j].start - 1].x;
			double xMid = polygonS[i].polyP[joinL[i].joinP[j].mid - 1].x;
			double xEnd = polygonS[i].polyP[joinL[i].joinP[j].end - 1].x;

			double yStart = polygonS[i].polyP[joinL[i].joinP[j].start - 1].y;
			double yMid = polygonS[i].polyP[joinL[i].joinP[j].mid - 1].y;
			double yEnd = polygonS[i].polyP[joinL[i].joinP[j].end - 1].y;
			joinL[i].joinP[j].sm = ddaG(xStart, xMid, yStart, yMid, 
				polygonS[i].polyP[joinL[i].joinP[j].start - 1].pixel,
				polygonS[i].polyP[joinL[i].joinP[j].mid - 1].pixel);
			joinL[i].joinP[j].se = ddaG(xStart, xEnd, yStart, yEnd, polygonS[i].polyP[joinL[i].joinP[j].start - 1].pixel,
				polygonS[i].polyP[joinL[i].joinP[j].end - 1].pixel);
			joinL[i].joinP[j].me = ddaG(xEnd, xMid, yEnd, yMid, polygonS[i].polyP[joinL[i].joinP[j].end - 1].pixel,
				polygonS[i].polyP[joinL[i].joinP[j].mid - 1].pixel);
		}
		
		/*for (unsigned int j = 0; j < joinL[i].joinP.size(); j++) {
			//thread th1(rasterization, joinL[i].joinP[j], polygonL[i]);
			rasterization(joinL[i].joinP[j], polygonL[i]);
		//	cout << "here" << endl;
		}*/
		/*
		for (int j = 0; j < joinL[i].joinP.size(); j++) {
			map<int, PNT> tempMap;
			map<int, PNT> tempMap2;
			if (joinL[i].joinP[j].sm.size() > joinL[i].joinP[j].se.size() &&
				joinL[i].joinP[j].sm.size() > joinL[i].joinP[j].me.size()) {
				tempMap = joinL[i].joinP[j].sm;
				tempMap2 = joinL[i].joinP[j].se;
			}	
			else if (joinL[i].joinP[j].se.size() > joinL[i].joinP[j].sm.size() &&
				joinL[i].joinP[j].se.size() > joinL[i].joinP[j].me.size()) {
				tempMap = joinL[i].joinP[j].se;
				tempMap2 = joinL[i].joinP[j].sm;
			}
			else {
				tempMap = joinL[i].joinP[j].me;
				tempMap2 = joinL[i].joinP[j].se;
			}
				
			map<int, PNT>::iterator it = tempMap.begin();
			map<int, PNT>::iterator it2 = tempMap2.begin();
			double t = 0;
			float slope = (float)tempMap2.size() / (float)tempMap.size();
			cout << tempMap2.size()<< 3/8 << tempMap2.size() / tempMap.size() << endl;
			while (it != tempMap.end()){
				t += slope;
				if (t - (int)t >= 0.5 && it2 != tempMap2.end())
					it2++;
				it++;
				
			}
		}*/
		cout << "dpne" << endl;
		glDisable(GL_SCISSOR_TEST);
		glFlush();
	}
}

void gouraudShadeY() {
	polygonL[0] = translate3D(polygonL[0], 0, 0, 200);
	glEnable(GL_SCISSOR_TEST);
	glViewport(grid_width / 2, 0, grid_width / 2, grid_height / 2);
	glScissor(grid_width / 2, 0, grid_width / 2, grid_height / 2);
	drawBox();
	
	//POLYGON tempPoly = normalize(polygonL[0], joinL[0]);
	//ambience, light intensity, distance, eye point, light colour, light location
	//phongShade(am, 0.2, 5000, f, light, l, polygonL[0]);
	vector<POLYGON> polygonS = painterAlgX(polygonL);
	for (unsigned int i = 0; i < joinL.size(); i++) {
		//polygonL[i] = normalize(polygonL[i], joinL[i]);

		for (unsigned int j = 0; j < joinL[i].joinP.size(); j++) {
			double xStart = polygonS[i].polyP[joinL[i].joinP[j].start - 1].y;
			double xMid = polygonS[i].polyP[joinL[i].joinP[j].mid - 1].y;
			double xEnd = polygonS[i].polyP[joinL[i].joinP[j].end - 1].y;

			double yStart = polygonS[i].polyP[joinL[i].joinP[j].start - 1].z;
			double yMid = polygonS[i].polyP[joinL[i].joinP[j].mid - 1].z;
			double yEnd = polygonS[i].polyP[joinL[i].joinP[j].end - 1].z;
			joinL[i].joinP[j].sm = ddaG(xStart, xMid, yStart, yMid,
				polygonS[i].polyP[joinL[i].joinP[j].start - 1].pixel,
				polygonS[i].polyP[joinL[i].joinP[j].mid - 1].pixel);
			joinL[i].joinP[j].se = ddaG(xStart, xEnd, yStart, yEnd, polygonS[i].polyP[joinL[i].joinP[j].start - 1].pixel,
				polygonS[i].polyP[joinL[i].joinP[j].end - 1].pixel);
			joinL[i].joinP[j].me = ddaG(xEnd, xMid, yEnd, yMid, polygonS[i].polyP[joinL[i].joinP[j].end - 1].pixel,
				polygonS[i].polyP[joinL[i].joinP[j].mid - 1].pixel);
		}

		/*for (unsigned int j = 0; j < joinL[i].joinP.size(); j++) {
			//thread th1(rasterization, joinL[i].joinP[j], polygonL[i]);
			rasterization(joinL[i].joinP[j], polygonL[i]);
			//	cout << "here" << endl;
		}*/
		
		cout << "dpne" << endl;
		glDisable(GL_SCISSOR_TEST);
		glFlush();
	}
}

void gouraudShadeZ() {
	//polygonL[0] = translate3D(polygonL[0], 0, 0, 200);
	glEnable(GL_SCISSOR_TEST);
	glViewport(0, grid_height / 2, grid_width / 2, grid_height / 2);
	glScissor(0, grid_height / 2, grid_width / 2, grid_height / 2);
	drawBox();

	//POLYGON tempPoly = normalize(polygonL[0], joinL[0]);
	//ambience, light intensity, distance, eye point, light colour, light location
	//phongShade(am, 0.2, 5000, f, light, l, polygonL[0]);
	vector<POLYGON> polygonS = painterAlgY(polygonL);
	for (unsigned int i = 0; i < joinL.size(); i++) {
		//polygonL[i] = normalize(polygonL[i], joinL[i]);

		for (unsigned int j = 0; j < joinL[i].joinP.size(); j++) {
			double xStart = polygonS[i].polyP[joinL[i].joinP[j].start - 1].x;
			double xMid = polygonS[i].polyP[joinL[i].joinP[j].mid - 1].x;
			double xEnd = polygonS[i].polyP[joinL[i].joinP[j].end - 1].x;

			double yStart = polygonS[i].polyP[joinL[i].joinP[j].start - 1].z;
			double yMid = polygonS[i].polyP[joinL[i].joinP[j].mid - 1].z;
			double yEnd = polygonS[i].polyP[joinL[i].joinP[j].end - 1].z;
			joinL[i].joinP[j].sm = ddaG(xStart, xMid, yStart, yMid,
				polygonS[i].polyP[joinL[i].joinP[j].start - 1].pixel,
				polygonS[i].polyP[joinL[i].joinP[j].mid - 1].pixel);
			joinL[i].joinP[j].se = ddaG(xStart, xEnd, yStart, yEnd, polygonS[i].polyP[joinL[i].joinP[j].start - 1].pixel,
				polygonS[i].polyP[joinL[i].joinP[j].end - 1].pixel);
			joinL[i].joinP[j].me = ddaG(xEnd, xMid, yEnd, yMid, polygonS[i].polyP[joinL[i].joinP[j].end - 1].pixel,
				polygonS[i].polyP[joinL[i].joinP[j].mid - 1].pixel);
		}

		/*for (unsigned int j = 0; j < joinL[i].joinP.size(); j++) {
			//thread th1(rasterization, joinL[i].joinP[j], polygonL[i]);
			rasterization(joinL[i].joinP[j], polygonL[i]);
		//	cout << "here" << endl;
		}*/

		glDisable(GL_SCISSOR_TEST);
		glFlush();
	}
}

map<int, PNT> ddaG(double xstart, double xend, double ystart, double yend, PIXEL startPix, PIXEL endPix) {
	map<int, PNT> pixVec;
	double ydiff = yend - ystart;
	double xdiff = xend - xstart;
	int step = 0;
	if (abs(xdiff) > abs(ydiff)) {
		step = abs(xdiff);
	}
	else {
		step = abs(ydiff);
	}

	double xinc = xdiff / step;
	double yinc = ydiff / step;

	double x = xstart;
	double y = ystart;
	for (int i = 0; i <= step; i++) {
		
		PIXEL temp = interpolate(startPix, endPix, y, ystart, yend);
		PNT tempP;
		tempP.pixel = temp;
		tempP.x = x;
		tempP.y = y;
		int yy = (int)(y + 0.5 - (y < 0));
		
		pixVec.insert(pair<int, PNT>(yy, tempP));


		glColor3f(startPix.red, startPix.green, startPix.blue);  //sets the current drawing (foreground) color to blue
		glPointSize(0.5);  //sets the size of points to be drawn (in pixel)
		glBegin(GL_POINTS); //writes pixels on fram buffer with current drawing color
		glVertex2i(x, y); //set vertex
		glEnd();
		x = x + xinc;
		y = y + yinc;
	}
	//cout << "here" << endl;
	glFlush();
	return pixVec;
}

PIXEL interpolate(PIXEL startPix, PIXEL endPix, float p, float start, float end) {
	PIXEL temp;
	temp.blue = startPix.blue * ((p - end) / (start - end)) +
		endPix.blue * ((start - p) / (start - end));

	temp.red = startPix.red * ((p - end) / (start - end)) +
		endPix.red * ((start - p) / (start - end));

	temp.green = startPix.green * ((p - end) / (start - end)) +
		endPix.green * ((start - p) / (start - end));
	return temp;
}

void rasterization(JOIN join, POLYGON polygon) {

	int xStart = min(polygon.polyP[join.end - 1].x,
		min(polygon.polyP[join.start - 1].x, polygon.polyP[join.mid - 1].x));
	int yStart = min(polygon.polyP[join.end - 1].y,
		min(polygon.polyP[join.start - 1].y, polygon.polyP[join.mid - 1].y));
	int xEnd = max(polygon.polyP[join.end - 1].x,
		max(polygon.polyP[join.start - 1].x, polygon.polyP[join.mid - 1].x));
	int yEnd = max(polygon.polyP[join.end - 1].y,
		max(polygon.polyP[join.start - 1].y, polygon.polyP[join.mid - 1].y));

	bool in = false;
	while (xStart < xEnd && yStart < yEnd) {
		while (xStart < xEnd) {
			PNT p;
			p.x = xStart;
			p.y = yStart;
			int inside = isInside(join, 3, p, polygon);
			if (inside > 0) {//is inside
				if (inside == 1) {// not on segment
					int line = checkBetweenLine(join, p, polygon);
					PNT startP;
					PIXEL startPix;
					PNT endP;
					PIXEL endPix;
					
					if (line == 3) {
						startP = join.sm.find(yStart)->second;
						startPix = startP.pixel;
						endP = join.se.find(yStart)->second;
						endPix = endP.pixel;
						p.pixel = interpolate(startPix, endPix, p.x, startP.x, endP.x);
					}//sm and se
					else if(line == 4){
						startP = join.sm.find(yStart)->second;
						startPix = startP.pixel;
						endP = join.me.find(yStart)->second;
						endPix = endP.pixel;
						p.pixel = interpolate(startPix, endPix, p.x, startP.x, endP.x);
					}//sm and me
					else if(line == 5){
						startP = join.me.find(yStart)->second;
						startPix = startP.pixel;
						endP = join.se.find(yStart)->second;
						endPix = endP.pixel;
						p.pixel = interpolate(startPix, endPix, p.x, startP.x, endP.x);
					}//se and me
					else if (line == 6){
						startP = join.me.find(yStart)->second;
						startPix = startP.pixel;
						endP = join.se.find(yStart)->second;
						endPix = endP.pixel;
						p.pixel = interpolate(startPix, endPix, p.x, startP.x, endP.x);
						
					}
					else {
						cout << "error in finding point between line" << endl;
					}
					glColor3f(p.pixel.red,p.pixel.green, p.pixel.blue);  //sets the current drawing (foreground) color to blue
					glPointSize(1);  //sets the size of points to be drawn (in pixel)
					glBegin(GL_POINTS);
					glVertex2i(round(xStart), round(yStart)); //set vertex
					glEnd();
				}
			}
			xStart+= 1;
		}
		xStart = min(polygon.polyP[join.end - 1].x,
			min(polygon.polyP[join.start - 1].x, polygon.polyP[join.mid - 1].x));;

		yStart+= 1;
	}
	//glFlush();
}


void rasterizationY(JOIN join, POLYGON polygon) {

	int xStart = min(polygon.polyP[join.end - 1].y,
		min(polygon.polyP[join.start - 1].y, polygon.polyP[join.mid - 1].y));
	int yStart = min(polygon.polyP[join.end - 1].z,
		min(polygon.polyP[join.start - 1].z, polygon.polyP[join.mid - 1].z));
	int xEnd = max(polygon.polyP[join.end - 1].y,
		max(polygon.polyP[join.start - 1].y, polygon.polyP[join.mid - 1].y));
	int yEnd = max(polygon.polyP[join.end - 1].z,
		max(polygon.polyP[join.start - 1].z, polygon.polyP[join.mid - 1].z));

	bool in = false;
	while (xStart < xEnd && yStart < yEnd) {
		while (xStart < xEnd) {
			PNT p;
			p.x = xStart;
			p.y = yStart;
			int inside = isInside(join, 3, p, polygon);
			if (inside > 0) {//is inside
				if (inside == 1) {// not on segment
					int line = checkBetweenLine(join, p, polygon);
					PNT startP;
					PIXEL startPix;
					PNT endP;
					PIXEL endPix;

					if (line == 3) {
						startP = join.sm.find(yStart)->second;
						startPix = startP.pixel;
						endP = join.se.find(yStart)->second;
						endPix = endP.pixel;
						p.pixel = interpolate(startPix, endPix, p.x, startP.x, endP.x);
					}//sm and se
					else if (line == 4) {
						startP = join.sm.find(yStart)->second;
						startPix = startP.pixel;
						endP = join.me.find(yStart)->second;
						endPix = endP.pixel;
						p.pixel = interpolate(startPix, endPix, p.x, startP.x, endP.x);
					}//sm and me
					else if (line == 5) {
						startP = join.me.find(yStart)->second;
						startPix = startP.pixel;
						endP = join.se.find(yStart)->second;
						endPix = endP.pixel;
						p.pixel = interpolate(startPix, endPix, p.x, startP.x, endP.x);
					}//se and me
					else if (line == 6) {
						startP = join.me.find(yStart)->second;
						startPix = startP.pixel;
						endP = join.se.find(yStart)->second;
						endPix = endP.pixel;
						p.pixel = interpolate(startPix, endPix, p.x, startP.x, endP.x);

					}
					else {
						cout << "error in finding point between line" << endl;
					}
					glColor3f(p.pixel.red, p.pixel.green, p.pixel.blue);  //sets the current drawing (foreground) color to blue
					glPointSize(1);  //sets the size of points to be drawn (in pixel)
					glBegin(GL_POINTS);
					glVertex2i(round(xStart), round(yStart)); //set vertex
					glEnd();
				}
			}
			xStart += 1;
		}
		xStart = min(polygon.polyP[join.end - 1].y,
			min(polygon.polyP[join.start - 1].y, polygon.polyP[join.mid - 1].y));;

		yStart += 1;
	}
	//glFlush();
}

int checkBetweenLine(JOIN join, PNT p, POLYGON polygon) {
	int res = 0;
	if (p.y >= min(polygon.polyP[join.start - 1].y, polygon.polyP[join.mid - 1].y) 
		&& p.y <= max(polygon.polyP[join.start - 1].y, polygon.polyP[join.mid - 1].y)) {
		res += 1;
	}
	if (p.y >= min(polygon.polyP[join.start - 1].y, polygon.polyP[join.end - 1].y) && p.y <= max(polygon.polyP[join.start - 1].y, polygon.polyP[join.end - 1].y)) {
		res += 2;
	}
	if (p.y >= min(polygon.polyP[join.mid - 1].y, polygon.polyP[join.end - 1].y) && p.y <= max(polygon.polyP[join.mid - 1].y, polygon.polyP[join.end - 1].y)) {
		res += 3;
	}
	return res;
}



void halftone(JOIN join, POLYGON polygon) {

}

int isInside(JOIN join, int n, PNT p, POLYGON polygon){
	if (n < 3)  return false;
	vector<PNT> temp;
	temp.push_back(polygon.polyP[join.start - 1]);
	temp.push_back(polygon.polyP[join.mid - 1]);
	temp.push_back(polygon.polyP[join.end - 1]);
	PNT extreme = { 10000, p.y };
	int count = 0, i = 0;
	do {
		int next = (i + 1) % n;
		if (doIntersect(temp[i], temp[next], p, extreme)) {//0 is false 1 is on line 2 is inside
			
			if (orientation(temp[i], p, temp[next]) == 0) {
				if (onSegment(temp[i], p, temp[next])) return 2;
				else return 0;
			}
			count++;
		}
		i = next;
	} while (i != 0);

	if (count % 2 == 1) {//true
		return 1;
	}
	else return 0;
	
}

bool doIntersect(PNT p1, PNT q1, PNT p2, PNT q2){
	int o1 = orientation(p1, q1, p2);
	int o2 = orientation(p1, q1, q2);
	int o3 = orientation(p2, q2, p1);
	int o4 = orientation(p2, q2, q1);
	if (o1 != o2 && o3 != o4)
		return true;//not on segment

	if (o1 == 0 && onSegment(p1, p2, q1)) return true;
	if (o2 == 0 && onSegment(p1, q2, q1)) return true;
	if (o3 == 0 && onSegment(p2, p1, q2)) return true;
	if (o4 == 0 && onSegment(p2, q1, q2)) return true;
	return false; // Doesn't fall in any of the above cases
}

bool onSegment(PNT p, PNT q, PNT r){
	if (q.x <= max(p.x, r.x) && q.x >= min(p.x, r.x) &&
		q.y <= max(p.y, r.y) && q.y >= min(p.y, r.y))
		return true;
	return false;
}

int orientation(PNT p, PNT q, PNT r)
{
	int val = (q.y - p.y) * (r.x - q.x) - (q.x - p.x) * (r.y - q.y);

	if (val == 0) return 0;  // colinear
	return (val > 0) ? 1 : 2; // clock or counterclock wise
}

void phongShade(PIXEL ki, double il, double k, PNT f, PIXEL ks, PNT ls,POLYGON polygon) {
	//normalize();
	for (unsigned int i = 0; i < joinL.size(); i++) {
		for (unsigned int j = 0; j < joinL[i].joinP.size(); j++) {
			double spec = joinL[i].joinP[j].spec;
			//PNT normVec = normP(joinL[i].joinP[j], polygonL[i]);
			PNT temp = polygonL[i].polyP[joinL[i].joinP[j].start - 1];
			PNT lqs = lightvec(ls, temp);
			PNT vVec = lightvec(temp, f);
			

			double ref = pow(dotP(reflectVec(temp, ls), vVec), joinL[i].joinP[j].spec);
			double lightK = (mag(f, temp) + k);

			polygonL[i].polyP[joinL[i].joinP[j].start - 1].pixel.blue = 
				abs(ki.blue + (il / lightK) * (dotP(temp, lqs) * temp.pixel.blue + ks.blue * ref));
			polygonL[i].polyP[joinL[i].joinP[j].start - 1].pixel.red =
				ki.red + (il / lightK) * (dotP(temp, lqs) * temp.pixel.red + ks.red * ref);
			polygonL[i].polyP[joinL[i].joinP[j].start - 1].pixel.green =
				ki.green + (il / lightK) * (dotP(temp, lqs) * temp.pixel.green + ks.green * ref);

			temp = polygonL[i].polyP[joinL[i].joinP[j].mid - 1];
			lqs = lightvec(temp, ls);
			vVec = lightvec(f, temp);
			lightK = (mag(f, temp) + k);
			ref = pow(dotP(reflectVec(temp, ls), vVec), joinL[i].joinP[j].spec);
			polygonL[i].polyP[joinL[i].joinP[j].mid - 1].pixel.blue =
				abs(ki.blue + (il / lightK) * (dotP(temp, lqs) * temp.pixel.blue + ks.blue * ref));
			polygonL[i].polyP[joinL[i].joinP[j].mid - 1].pixel.red =
				ki.red + (il / lightK) * (dotP(temp, lqs) * temp.pixel.red + ks.red * ref);
			polygonL[i].polyP[joinL[i].joinP[j].mid - 1].pixel.green =
				ki.green + (il / lightK) * (dotP(temp, lqs) * temp.pixel.green + ks.green * ref);

			temp = polygonL[i].polyP[joinL[i].joinP[j].end - 1];
			lqs = lightvec(temp, ls);
			vVec = lightvec(f, temp);
			lightK = (mag(f, temp) + k);
			ref = pow(dotP(reflectVec(temp, ls), vVec), joinL[i].joinP[j].spec);
			polygonL[i].polyP[joinL[i].joinP[j].end - 1].pixel.blue =
				abs(ki.blue + (il / lightK) * (dotP(temp, lqs) * temp.pixel.blue + ks.blue * ref));
			polygonL[i].polyP[joinL[i].joinP[j].end - 1].pixel.red =
				ki.red + (il / lightK) * (dotP(temp, lqs) * temp.pixel.red + ks.red * ref);
			polygonL[i].polyP[joinL[i].joinP[j].end - 1].pixel.green =
				ki.green + (il / lightK) * (dotP(temp, lqs) * temp.pixel.green + ks.green * ref);

			glColor3f(polygonL[i].polyP[joinL[i].joinP[j].end - 1].pixel.red, 
				polygonL[i].polyP[joinL[i].joinP[j].end - 1].pixel.green, 
				polygonL[i].polyP[joinL[i].joinP[j].end - 1].pixel.blue);  //sets the current drawing (foreground) color to blue
			/*glPointSize(1);  //sets the size of points to be drawn (in pixel)
			glBegin(GL_POINTS); //writes pixels on fram buffer with current drawing color
			glVertex2i(polygonL[i].polyP[joinL[i].joinP[j].end - 1].x, polygonL[i].polyP[joinL[i].joinP[j].end - 1].y); //set vertex
			glEnd();
			glFlush();*/
		}
	}
}


PNT reflectVec(PNT p, PNT l) {
	PNT reflec;
	double dotPO = dotP(p, l);
	
	reflec.x = (dotPO * 2 * p.normX - l.x);
	reflec.y = (dotPO * 2 * p.normY - l.y);
	reflec.z = (dotPO * 2 * p.normZ - l.z);
	double magg = mag(reflec, l);
	reflec.x = reflec.x / magg;
	reflec.y = reflec.y / magg;
	reflec.z = reflec.z / magg;
	return reflec;
}

double dotP(PNT p, PNT l) {
	return (p.x * l.x + p.y * l.y + p.z * l.z);
}

PNT lightvec(PNT p, PNT l) {
	PNT temp;
	double magg = sqrt(pow(p.normX - l.x, 2) + pow(p.normY - l.y, 2) + pow(p.normZ - l.z, 2));
	temp.x = (p.normX - l.x) / magg;
	temp.y = (p.normY - l.y) / magg;
	temp.z = (p.normZ - l.z) / magg;
	return temp;
}

PNT normP(JOIN join, POLYGON polygon) {
	PNT p1, p2, crossP;
	p2.x = polygon.polyP[join.end - 1].x - polygon.polyP[join.start - 1].x;
	p2.y = polygon.polyP[join.end - 1].y - polygon.polyP[join.start - 1].y;
	p2.z = polygon.polyP[join.end - 1].z - polygon.polyP[join.start - 1].z;

	p1.x = polygon.polyP[join.mid - 1].x - polygon.polyP[join.start - 1].x;
	p1.y = polygon.polyP[join.mid - 1].y - polygon.polyP[join.start - 1].y;
	p1.z = polygon.polyP[join.mid - 1].z - polygon.polyP[join.start - 1].z;

	double magg = mag(p1, p2);
	crossP.x = (p1.y * p2.z - p1.z * p2.y) / magg;
	crossP.y = (p1.z * p2.x - p1.z * p2.z) / magg;
	crossP.z = (p1.z * p2.y - p1.y * p2.x) / magg;
	return crossP;
}

double mag(PNT p, PNT l) {
	return sqrt((p.x - l.x) * (p.x - l.x) +
		(p.y - l.y) * (p.y - l.y) +
		(p.x - l.z) * (p.x - l.z));
}

void reshape(int w, int h) {
	grid_width = w;
	grid_height = h;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(-1.0, 1.0, -1.0, 1.0, 1.5, 20.0);
	glMatrixMode(GL_MODELVIEW);
}

void drawXY(POLYGON polygon, int joinNum) {

	glViewport(0, 0, grid_width / 2, grid_height / 2);
	glScissor(0, 0, grid_width / 2, grid_height / 2);
	drawBox();

	/*for (unsigned int i = 0; i < joinL[joinNum].joinP.size(); i++) {
		PNT ps, pe, pm;
		
		ps = polygon.polyP[joinL[joinNum].joinP[i].start - 1];


		pe = polygon.polyP[joinL[joinNum].joinP[i].end - 1];

		pm = polygon.polyP[joinL[joinNum].joinP[i].mid - 1];
		//cout << ps.pixel.blue << endl;

		ddaG(ps.x, pe.x, ps.y, pe.y, ps.pixel, pe.pixel);
		ddaG(ps.x, pm.x, ps.y, pm.y, ps.pixel, pm.pixel);
		ddaG(pm.x, pe.x, pm.y, pe.y, pm.pixel, pe.pixel);
	}*/
}

void drawXZ(POLYGON polygon, int joinNum) {

	glViewport(0, grid_height / 2, grid_width / 2, grid_height / 2);
	glScissor(0, grid_height / 2, grid_width / 2, grid_height / 2);
	drawBox();

	/*for (unsigned int i = 0; i < joinL[joinNum].joinP.size(); i++) {
		PNT ps, pe, pm;

		ps.y = polygon.polyP[joinL[joinNum].joinP[i].start - 1].y;
		ps.x = polygon.polyP[joinL[joinNum].joinP[i].start - 1].x;

		pe.y = polygon.polyP[joinL[joinNum].joinP[i].end - 1].y;
		pe.x = polygon.polyP[joinL[joinNum].joinP[i].end - 1].z;

		pm.y = polygon.polyP[joinL[joinNum].joinP[i].mid - 1].y;
		pm.x = polygon.polyP[joinL[joinNum].joinP[i].mid - 1].x;

		dda(ps.x, pe.x, ps.y, pe.y);
		dda(ps.x, pm.x, ps.y, pm.y);
		dda(pm.x, pe.x, pm.y, pe.y);
	}*/
}

void drawYZ(POLYGON polygon, int joinNum) {

	glViewport(grid_width / 2, 0, grid_width / 2, grid_height / 2);
	glScissor(grid_width / 2, 0, grid_width / 2, grid_height / 2);
	drawBox();

	/*for (unsigned int i = 0; i < joinL[joinNum].joinP.size(); i++) {
		PNT ps, pe, pm;

		ps.y = polygon.polyP[joinL[joinNum].joinP[i].start - 1].y;
		ps.x = polygon.polyP[joinL[joinNum].joinP[i].start - 1].x;

		pe.y = polygon.polyP[joinL[joinNum].joinP[i].end - 1].y;
		pe.x = polygon.polyP[joinL[joinNum].joinP[i].end - 1].z;

		pm.y = polygon.polyP[joinL[joinNum].joinP[i].mid - 1].y;
		pm.x = polygon.polyP[joinL[joinNum].joinP[i].mid - 1].x;

		dda(ps.x, pe.x, ps.y, pe.y);
		dda(ps.x, pm.x, ps.y, pm.y);
		dda(pm.x, pe.x, pm.y, pe.y);
	}*/
}

void rotate3D(POLYGON polygon, PNT p1, PNT p2, int radianAngle) {
	Matrix4x4 matR;
	matrix4x4SetIdentity(matRot);
	float axisVectLength = sqrt((p2.x - p1.x) * (p2.x - p1.x) +
		(p2.y - p1.y) * (p2.y - p1.y) +
		(p2.z - p1.z) * (p2.z - p1.z));

	float cosA = cos(radianAngle);
	float oneC = 1 - cosA;
	float sinA = sin(radianAngle);
	float ux = (p2.x - p1.x) / axisVectLength;
	float uy = (p2.y - p1.y) / axisVectLength;
	float uz = (p2.z - p1.z) / axisVectLength;
	/* Set up translation matrix for moving p1 to origin.*/
	rotTranslate3D(-p1.x, -p1.y, -p1.z);
	/* Initialize matR to identity matrix.*/
	matrix4x4SetIdentity(matR);
	matR[0][0] = ux * uy * oneC + cosA;
	matR[0][1] = ux * uy * oneC - uz * sinA;
	matR[0][2] = ux * uz * oneC + uy * sinA;
	matR[1][0] = uy * ux * oneC + uz * sinA;
	matR[1][1] = uy * ux * oneC + cosA;
	matR[1][2] = uy * uz * oneC - ux * sinA;
	matR[2][0] = uz * ux * oneC - uy * sinA;
	matR[2][1] = uz * uy * oneC + ux * sinA;
	matR[2][2] = uz * uz * oneC + cosA;
	matrix4x4PreMultiply(matR, matRot);
	rotTranslate3D(p1.x, p1.y, p1.z);
}

POLYGON translate3D(POLYGON polygon, double tx, double ty, double tz) {
	Matrix4x4 matTransl3D;
	/* Initialize translation matrix to identity.*/
	matrix4x4SetIdentity(matTransl3D);

	matTransl3D[0][3] = tx;
	matTransl3D[1][3] = ty;
	matTransl3D[2][3] = tz;

	for (unsigned int i = 0; i < polygon.polyP.size(); i++) {
		polygon.polyP[i] = matrixPreMultiply(matTransl3D, polygon.polyP[i]);
	}

	return polygon;
}

void rotTranslate3D(double tx, double ty, double tz) {
	Matrix4x4 matTransl3D;
	/* Initialize translation matrix to identity.*/
	matrix4x4SetIdentity(matTransl3D);
	matTransl3D[0][3] = tx;
	matTransl3D[1][3] = ty;
	matTransl3D[2][3] = tz;
	/* Concatenate translation matrix with matRot.*/
	matrix4x4PreMultiply(matTransl3D, matRot);
}

void matrix4x4SetIdentity(Matrix4x4 matIdent4x4) {
	int row, col;
	for (row = 0; row < 4; row++)
		for (col = 0; col < 4; col++)
			matIdent4x4[row][col] = (row == col);
}

PNT matrixPreMultiply(Matrix4x4 m1, PNT m2) {
	int row, col;
	int matTemp[4] = {};
	for (row = 0; row < 4; row++)
		for (col = 0; col < 4; col++)
			matTemp[row] = m1[row][0] * m2.x + m1[row][1] *
			m2.y + m1[row][2] * m2.z +
			m1[row][3] * 1;
	m2.x = matTemp[0];
	m2.y = matTemp[1];
	m2.z = matTemp[2];
	return m2;
}
/* Premultiply matrix m1 by matrix m2, store result in m2. */
void matrix4x4PreMultiply(Matrix4x4 m1, Matrix4x4 m2) {
	GLint row, col;
	Matrix4x4 matTemp;
	for (row = 0; row < 4; row++)
		for (col = 0; col < 4; col++)
			matTemp[row][col] = m1[row][0] * m2[0][col] + m1[row][1] *
			m2[1][col] + m1[row][2] * m2[2][col] +
			m1[row][3] * m2[3][col];
	for (row = 0; row < 4; row++)
		for (col = 0; col < 4; col++)
			m2[row][col] = matTemp[row][col];

}

void dda(double xstart, double xend, double ystart, double yend) {
	double ydiff = yend - ystart;
	double xdiff = xend - xstart;
	int step = 0;
	if (abs(xdiff) > abs(ydiff)) {
		step = abs(xdiff);
	}
	else {
		step = abs(ydiff);
	}

	double xinc = xdiff / step;
	double yinc = ydiff / step;

	double x = xstart;
	double y = ystart;
	for (int i = 0; i < step; i++) {
		x = x + xinc;
		y = y + yinc;

		glColor3f(0, 0, 1);  //sets the current drawing (foreground) color to blue
		glPointSize(1);  //sets the size of points to be drawn (in pixel)
		glBegin(GL_POINTS); //writes pixels on fram buffer with current drawing color
		glVertex2i(x, y); //set vertex
		glEnd();
	}
}

POLYGON scale(POLYGON polygon1, double factor, double vertex) {
	PNT cent = centre(polygon1);
	matrix4x4SetIdentity(matRot);
	for (int i = 0; i < vertex; i++) {
		polygon1.polyP[i].x = factor * polygon1.polyP[i].x + (1 - factor) * cent.x;
		polygon1.polyP[i].y = factor * polygon1.polyP[i].y + (1 - factor) * cent.y;
		polygon1.polyP[i].z = factor * polygon1.polyP[i].z + (1 - factor) * cent.z;
	}
	return polygon1;
}

PNT centre(POLYGON polygon) {
	PNT p;
	p.x = polygon.polyP[0].x;
	p.y = polygon.polyP[0].y;
	p.z = polygon.polyP[0].z;
	for (unsigned int i = 1; i < polygon.polyP.size(); i++) {
		p.x = (p.x + polygon.polyP[i].x);
		p.y = (p.y + polygon.polyP[i].y);
		p.z = (p.z + polygon.polyP[i].z);
	}
	p.x = p.x / polygon.polyP.size();
	p.y = p.y / polygon.polyP.size();
	p.z = p.z / polygon.polyP.size();
	return p;
}

void setup_illumination()
{
	// Intialise and set lighting parameters
	GLfloat light_pos[] = { 1.0, 1.0, 1.0, 0.0 };
	GLfloat light_ka[] = { 0.2, 0.2, 0.2, 1.0 };
	GLfloat light_kd[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat light_ks[] = { 1.0, 1.0, 1.0, 1.0 };

	glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ka);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_kd);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_ks);

	// Initialise and set material parameters
	GLfloat material_ka[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat material_kd[] = { 0.43, 0.47, 0.54, 1.0 };
	GLfloat material_ks[] = { 0.33, 0.33, 0.52, 1.0 };
	GLfloat material_ke[] = { 0.0, 0.0, 0.0, 0.0 };
	GLfloat material_se[] = { 10.0 };

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, material_ka);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, material_kd);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, material_ks);
	glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, material_ke);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, material_se);

	// Smooth shading
	glShadeModel(GL_SMOOTH);

	// Enable lighting
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
}
