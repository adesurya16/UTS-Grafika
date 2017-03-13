#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <cstdio>
#include <termios.h>
#include <math.h>
#include <thread>
#include <vector>
#include "Object.h"


using namespace std;

#define HEIGHT 650 //HEIGHT jangan panjang panjang kan ada 2 layar satu untuk mini satu zoom in/out
#define WIDTH 650

struct fb_var_screeninfo vinfo;
struct fb_fix_screeninfo finfo;
char *fbp = 0;

bool listA = true;
bool listB = true;

//matriks mini map
int redPixelMatrix[WIDTH][HEIGHT];
int greenPixelMatrix[WIDTH][HEIGHT];
int bluePixelMatrix[WIDTH][HEIGHT];

//matriks zoom map
int redPixelMatrixZoomMap[WIDTH][HEIGHT];
int greenPixelMatrixZoomMap[WIDTH][HEIGHT];
int bluePixelMatrixZoomMap[WIDTH][HEIGHT];


int posX;
int posY;
int lastCorrectState = 's';
bool exploded = false;

void clearMatrix() {
	for (int i = 0; i < WIDTH; ++i)
	{
		for (int j = 0; j < HEIGHT; ++j)
		{
			redPixelMatrix[i][j] = 0;
			greenPixelMatrix[i][j] = 0;
			bluePixelMatrix[i][j] = 0;
		}
	}

	for (int i = 0; i < WIDTH; ++i)
	{
		for (int j = 0; j < HEIGHT; ++j)
		{
			redPixelMatrixZoomMap[i][j] = 0;
			greenPixelMatrixZoomMap[i][j] = 0;
			bluePixelMatrixZoomMap[i][j] = 0;
		}
	}
}

void drawWhitePoint2(int x1, int y1) {
	redPixelMatrixZoomMap[x1][y1] = 255;
	greenPixelMatrixZoomMap[x1][y1] = 255;
	bluePixelMatrixZoomMap[x1][y1] = 255;
}

void drawWhitePoint(int x1, int y1) {
    if (x1 < WIDTH && x1 >= 0 && y1 >=0 && y1 < HEIGHT) {
        redPixelMatrix[x1][y1] = 255;
        greenPixelMatrix[x1][y1] = 255;
        bluePixelMatrix[x1][y1] = 255;
    }
}

void drawRedPoint(int x1,int y1){
	redPixelMatrix[x1][y1] = 255;
	greenPixelMatrix[x1][y1] = 0;
	bluePixelMatrix[x1][y1] = 0;
}

void drawBlackPoint(int x1,int y1){
	redPixelMatrix[x1][y1] = 0;
	greenPixelMatrix[x1][y1] = 0;
	bluePixelMatrix[x1][y1] = 0;
}

bool drawWhiteLine(int x1, int y1, int x2, int y2) {
	//Than kode lu gua benerin dikit di sini, harusnya ngk usah pake absolut
	bool ret = false;

	int deltaX = x2 - x1;
	int deltaY = y2 - y1;
	int ix = deltaX > 0 ? 1 : -1;
	int iy = deltaY > 0 ? 1 : -1;
	deltaX = abs(deltaX);
	deltaY = abs(deltaY);

	int x = x1;
	int y = y1;

	drawWhitePoint(x,y);

	if (deltaX >= deltaY) {
		int error = 2 * deltaY - deltaX;

		while (x != x2) {
			if ((error >= 0) && (error || (ix > 0)))
			{
				error -= deltaX;
				y += iy;
			}

			error += deltaY;
			x += ix;


			if (redPixelMatrix[x][y] == 255 && greenPixelMatrix[x][y] == 255 && bluePixelMatrix[x][y] == 255) {
				ret = true;
			}
			drawWhitePoint(x, y);
		}
	} else {
		int error = 2 * deltaX - deltaY;

		while (y != y2)
		{
			if ((error >= 0) && (error || (iy > 0)))
			{
				error -= deltaY;
				x += ix;
			}

			error += deltaX;
			y += iy;


			if (redPixelMatrix[x][y] == 255 && greenPixelMatrix[x][y] == 255 && bluePixelMatrix[x][y] == 255) {
				ret = true;
			}
			drawWhitePoint(x, y);
		}
	}
	return ret;
}

bool drawWhiteLine2(int x1, int y1, int x2, int y2) {
	//Than kode lu gua benerin dikit di sini, harusnya ngk usah pake absolut
	bool ret = false;

	int deltaX = x2 - x1;
	int deltaY = y2 - y1;
	int ix = deltaX > 0 ? 1 : -1;
	int iy = deltaY > 0 ? 1 : -1;
	deltaX = abs(deltaX);
	deltaY = abs(deltaY);

	int x = x1;
	int y = y1;

	drawWhitePoint2(x,y);

	if (deltaX >= deltaY) {
		int error = 2 * deltaY - deltaX;

		while (x != x2) {
			if ((error >= 0) && (error || (ix > 0)))
			{
				error -= deltaX;
				y += iy;
			}

			error += deltaY;
			x += ix;


			if (redPixelMatrixZoomMap[x][y] == 255 && greenPixelMatrixZoomMap[x][y] == 255 && bluePixelMatrixZoomMap[x][y] == 255) {
				ret = true;
			}
			drawWhitePoint2(x, y);
		}
	} else {
		int error = 2 * deltaX - deltaY;

		while (y != y2)
		{
			if ((error >= 0) && (error || (iy > 0)))
			{
				error -= deltaY;
				x += ix;
			}

			error += deltaX;
			y += iy;


			if (redPixelMatrixZoomMap[x][y] == 255 && greenPixelMatrixZoomMap[x][y] == 255 && bluePixelMatrixZoomMap[x][y] == 255) {
				ret = true;
			}
			drawWhitePoint2(x, y);
		}
	}
	return ret;
}

void drawBlackLine(int x1, int y1, int x2, int y2) {
    //Than kode lu gua benerin dikit di sini, harusnya ngk usah pake absolut
	int deltaX = x2 - x1;
	int deltaY = y2 - y1;
	int ix = deltaX > 0 ? 1 : -1;
	int iy = deltaY > 0 ? 1 : -1;
	deltaX = abs(deltaX);
	deltaY = abs(deltaY);

	int x = x1;
	int y = y1;

	drawBlackPoint(x,y);

	if (deltaX >= deltaY) {
		int error = 2 * deltaY - deltaX;

		while (x != x2) {
			if ((error >= 0) && (error || (ix > 0)))
			{
				error -= deltaX;
				y += iy;
			}

			error += deltaY;
			x += ix;

			drawBlackPoint(x, y);
		}
	} else {
		int error = 2 * deltaX - deltaY;

		while (y != y2)
		{
			if ((error >= 0) && (error || (iy > 0)))
			{
				error -= deltaY;
				x += ix;
			}

			error += deltaX;
			y += iy;

			drawBlackPoint(x, y);
		}
	}
}

void drawRedLine(int x1, int y1, int x2, int y2) {
    //Than kode lu gua benerin dikit di sini, harusnya ngk usah pake absolut
	int deltaX = x2 - x1;
	int deltaY = y2 - y1;
	int ix = deltaX > 0 ? 1 : -1;
	int iy = deltaY > 0 ? 1 : -1;
	deltaX = abs(deltaX);
	deltaY = abs(deltaY);

	int x = x1;
	int y = y1;

	drawRedPoint(x,y);

	if (deltaX >= deltaY) {
		int error = 2 * deltaY - deltaX;

		while (x != x2) {
			if ((error >= 0) && (error || (ix > 0)))
			{
				error -= deltaX;
				y += iy;
			}

			error += deltaY;
			x += ix;

			drawRedPoint(x, y);
		}
	} else {
		int error = 2 * deltaX - deltaY;

		while (y != y2)
		{
			if ((error >= 0) && (error || (iy > 0)))
			{
				error -= deltaY;
				x += ix;
			}

			error += deltaX;
			y += iy;

			drawRedPoint(x, y);
		}
	}
}

void drawWhitePointZoom(int x1, int y1) {
    if (x1 < WIDTH && x1 > 0 && y1 > 0 && y1 < HEIGHT) {
        redPixelMatrixZoomMap[x1][y1] = 255;
        greenPixelMatrixZoomMap[x1][y1] = 255;
        bluePixelMatrixZoomMap[x1][y1] = 255;
    }
}

bool drawWhiteLineZoom(int x1, int y1, int x2, int y2) {
	//Than kode lu gua benerin dikit di sini, harusnya ngk usah pake absolut
	bool ret = false;

	int deltaX = x2 - x1;
	int deltaY = y2 - y1;
	int ix = deltaX > 0 ? 1 : -1;
	int iy = deltaY > 0 ? 1 : -1;
	deltaX = abs(deltaX);
	deltaY = abs(deltaY);

	int x = x1;
	int y = y1;

	drawWhitePointZoom(x,y);

	if (deltaX >= deltaY) {
		int error = 2 * deltaY - deltaX;

		while (x != x2) {
			if ((error >= 0) && (error || (ix > 0)))
			{
				error -= deltaX;
				y += iy;
			}

			error += deltaY;
			x += ix;


			if (redPixelMatrixZoomMap[x][y] == 255 && greenPixelMatrixZoomMap[x][y] == 255 && bluePixelMatrixZoomMap[x][y] == 255) {
				ret = true;
			}
			drawWhitePointZoom(x, y);
		}
	} else {
		int error = 2 * deltaX - deltaY;

		while (y != y2)
		{
			if ((error >= 0) && (error || (iy > 0)))
			{
				error -= deltaY;
				x += ix;
			}

			error += deltaX;
			y += iy;


			if (redPixelMatrixZoomMap[x][y] == 255 && greenPixelMatrixZoomMap[x][y] == 255 && bluePixelMatrixZoomMap[x][y] == 255) {
				ret = true;
			}
			drawWhitePointZoom(x, y);
		}
	}
	return ret;
}
//List of Object
Object *listObject;
int nObject = 37; /////////////////////////////////////////////////////////////////////JANGAN LUPA UPDATE
void makeWorld() {

    int pointSize;
    Point *listPoint;
    listObject = (Object*) malloc(nObject*sizeof(Object));
    int idxObject = 0;

    //Diulang-ulang
    //////////////////////////////////////////////////////////////////////////////////////////
    pointSize = 8;
    listPoint =  (Point*)malloc(pointSize*sizeof(Point));
    int xKubus[8], yKubus[8];
	xKubus[0] = 156; yKubus[0] = 414;
	xKubus[1] = 159; yKubus[1] = 406;
	xKubus[2] = 166; yKubus[2] = 400;
	xKubus[3] = 167; yKubus[3] = 382;
	xKubus[4] = 194; yKubus[4] = 382;
	xKubus[5] = 195; yKubus[5] = 398;
	xKubus[6] = 201; yKubus[6] = 407;
	xKubus[7] = 204; yKubus[7] = 414;
	for(int i = 0; i < pointSize; i++) {
        listPoint[i].x = xKubus[i];
        listPoint[i].y = yKubus[i];
	}
	listObject[idxObject] = Object();
	listObject[idxObject].buildPolygon(listPoint, pointSize);
	idxObject++;
	free(listPoint);
	//////////////////////////////////////////////////////////////////////////////////////////
    pointSize = 4;
    listPoint = (Point*)malloc(pointSize*sizeof(Point));
    int xKotakKiri[4], yKotakKiri[4];
	xKotakKiri[0] = 75; yKotakKiri[0] = 415;
	xKotakKiri[1] = 71; yKotakKiri[1] = 406;
	xKotakKiri[2] = 100; yKotakKiri[2] = 396;
	xKotakKiri[3] = 103; yKotakKiri[3] = 405;
	for(int i = 0; i < pointSize; i++) {
        listPoint[i].x = xKotakKiri[i];
        listPoint[i].y = yKotakKiri[i];
	}
	listObject[idxObject] = Object();
	listObject[idxObject].buildPolygon(listPoint, pointSize);
	idxObject++;
	free(listPoint);
    //////////////////////////////////////////////////////////////////////////////////////////
    pointSize = 5;
    listPoint = (Point*)malloc(pointSize*sizeof(Point));
    int xKotakKanan[5], yKotakKanan[5];
	xKotakKanan[0] = 221; yKotakKanan[0] = 415;
	xKotakKanan[1] = 219; yKotakKanan[1] = 412;
	xKotakKanan[2] = 229; yKotakKanan[2] = 394;
	xKotakKanan[3] = 248; yKotakKanan[3] = 404;
	xKotakKanan[4] = 243; yKotakKanan[4] = 415;
	for(int i = 0; i < pointSize; i++) {
        listPoint[i].x = xKotakKanan[i];
        listPoint[i].y = yKotakKanan[i];
	}
	listObject[idxObject] = Object();
	listObject[idxObject].buildPolygon(listPoint, pointSize);
	idxObject++;
	free(listPoint);
    //////////////////////////////////////////////////////////////////////////////////////////
    pointSize = 5;
    listPoint = (Point*)malloc(pointSize*sizeof(Point));
    int xLapSil[5], yLapSil[5];
	xLapSil[0] = 61; yLapSil[0] = 395;
	xLapSil[1] = 57; yLapSil[1] = 385;
	xLapSil[2] = 95; yLapSil[2] = 371;
	xLapSil[3] = 141; yLapSil[3] = 355;
	xLapSil[4] = 141; yLapSil[4] = 362;
	for(int i = 0; i < pointSize; i++) {
        listPoint[i].x = xLapSil[i];
        listPoint[i].y = yLapSil[i];
	}
	listObject[idxObject] = Object();
	listObject[idxObject].buildPolygon(listPoint, pointSize);
	idxObject++;
	free(listPoint);
    //////////////////////////////////////////////////////////////////////////////////////////
    pointSize = 4;
    listPoint = (Point*)malloc(pointSize*sizeof(Point));
    int xLapSRKiri[4], yLapSRKiri[4];
	xLapSRKiri[0] = 210; yLapSRKiri[0] = 374;
	xLapSRKiri[1] = 211; yLapSRKiri[1] = 364;
	xLapSRKiri[2] = 216; yLapSRKiri[2] = 364;
	xLapSRKiri[3] = 215; yLapSRKiri[3] = 354;
	for(int i = 0; i < pointSize; i++) {
        listPoint[i].x = xLapSRKiri[i];
        listPoint[i].y = yLapSRKiri[i];
	}
	listObject[idxObject] = Object();
	listObject[idxObject].buildPolygon(listPoint, pointSize);
	idxObject++;
	free(listPoint);
    //////////////////////////////////////////////////////////////////////////////////////////
    pointSize = 4;
    listPoint = (Point*)malloc(pointSize*sizeof(Point));
    int xLapSRAtas[4], yLapSRAtas[4];
	xLapSRAtas[0] = 217; yLapSRAtas[0] = 355;
	xLapSRAtas[1] = 217; yLapSRAtas[1] = 364;
	xLapSRAtas[2] = 255; yLapSRAtas[2] = 372;
	xLapSRAtas[3] = 287; yLapSRAtas[3] = 384;
	for(int i = 0; i < pointSize; i++) {
        listPoint[i].x = xLapSRAtas[i];
        listPoint[i].y = yLapSRAtas[i];
	}
	listObject[idxObject] = Object();
	listObject[idxObject].buildPolygon(listPoint, pointSize);
	idxObject++;
	free(listPoint);
    //////////////////////////////////////////////////////////////////////////////////////////
    pointSize = 5;
    listPoint = (Point*)malloc(pointSize*sizeof(Point));
    int xCoklat[5], yCoklat[5];
	xCoklat[0] = 231; yCoklat[0] = 394;
	xCoklat[1] = 250; yCoklat[1] = 405;
	xCoklat[2] = 244; yCoklat[2] = 415;
	xCoklat[3] = 223; yCoklat[3] = 415;
	xCoklat[4] = 220; yCoklat[4] = 413;
	for(int i = 0; i < pointSize; i++) {
        listPoint[i].x = xCoklat[i];
        listPoint[i].y = yCoklat[i];
	}
	listObject[idxObject] = Object();
	listObject[idxObject].buildPolygon(listPoint, pointSize);
	idxObject++;
	free(listPoint);
    //////////////////////////////////////////////////////////////////////////////////////////
    pointSize = 4;
    listPoint = (Point*)malloc(pointSize*sizeof(Point));
    int xUngu[4], yUngu[4];
	xUngu[0] = 173; yUngu[0] = 343;
	xUngu[1] = 172; yUngu[1] = 349;
	xUngu[2] = 153; yUngu[2] = 349;
	xUngu[3] = 153; yUngu[3] = 343;
	for(int i = 0; i < pointSize; i++) {
        listPoint[i].x = xUngu[i];
        listPoint[i].y = yUngu[i];
	}
	listObject[idxObject] = Object();
	listObject[idxObject].buildPolygon(listPoint, pointSize);
	idxObject++;
	free(listPoint);
    //////////////////////////////////////////////////////////////////////////////////////////
    pointSize = 4;
    listPoint = (Point*)malloc(pointSize*sizeof(Point));
    int xCoklat1[4], yCoklat1[4];
	xCoklat1[0] = 210; yCoklat1[0] = 343;
	xCoklat1[1] = 210; yCoklat1[1] = 349;
	xCoklat1[2] = 190; yCoklat1[2] = 349;
	xCoklat1[3] = 190; yCoklat1[3] = 343;
	for(int i = 0; i < pointSize; i++) {
        listPoint[i].x = xCoklat1[i];
        listPoint[i].y = yCoklat1[i];
	}
	listObject[idxObject] = Object();
	listObject[idxObject].buildPolygon(listPoint, pointSize);
	idxObject++;
	free(listPoint);
    //////////////////////////////////////////////////////////////////////////////////////////
    pointSize = 16;
    listPoint = (Point*)malloc(pointSize*sizeof(Point));
    int xAlbar[16], yAlbar[16];
	xAlbar[0] = 149; yAlbar[0] = 311;
	xAlbar[1] = 149; yAlbar[1] = 315;
	xAlbar[2] = 155; yAlbar[2] = 315;
	xAlbar[3] = 155; yAlbar[3] = 317;
	xAlbar[4] = 158; yAlbar[4] = 317;
	xAlbar[5] = 158; yAlbar[5] = 326;
	xAlbar[6] = 151; yAlbar[6] = 326;
	xAlbar[7] = 151; yAlbar[7] = 335;
	xAlbar[8] = 122; yAlbar[8] = 335;
	xAlbar[9] = 122; yAlbar[9] = 328;
	xAlbar[10] = 115; yAlbar[10] = 328;
	xAlbar[11] = 115; yAlbar[11] = 317;
	xAlbar[12] = 119; yAlbar[12] = 317;
	xAlbar[13] = 119; yAlbar[13] = 314;
	xAlbar[14] = 125; yAlbar[14] = 315;
	xAlbar[15] = 125; yAlbar[15] = 311;
	for(int i = 0; i < pointSize; i++) {
        listPoint[i].x = xAlbar[i];
        listPoint[i].y = yAlbar[i];
	}
	listObject[idxObject] = Object();
	listObject[idxObject].buildPolygon(listPoint, pointSize);
	idxObject++;
	free(listPoint);
    //////////////////////////////////////////////////////////////////////////////////////////
    pointSize = 16;
    listPoint = (Point*)malloc(pointSize*sizeof(Point));
    int xAltim[16], yAltim[16];
	xAltim[0] = 237; yAltim[0] = 311;
	xAltim[1] = 237; yAltim[1] = 315;
	xAltim[2] = 242; yAltim[2] = 315;
	xAltim[3] = 242; yAltim[3] = 317;
	xAltim[4] = 247; yAltim[4] = 317;
	xAltim[5] = 247; yAltim[5] = 327;
	xAltim[6] = 240; yAltim[6] = 327;
	xAltim[7] = 240; yAltim[7] = 335;
	xAltim[8] = 211; yAltim[8] = 335;
	xAltim[9] = 211; yAltim[9] = 327;
	xAltim[10] = 204; yAltim[10] = 327;
	xAltim[11] = 204; yAltim[11] = 318;
	xAltim[12] = 208; yAltim[12] = 318;
	xAltim[13] = 208; yAltim[13] = 315;
	xAltim[14] = 213; yAltim[14] = 315;
	xAltim[15] = 213; yAltim[15] = 311;
	for(int i = 0; i < pointSize; i++) {
        listPoint[i].x = xAltim[i];
        listPoint[i].y = yAltim[i];
	}
	listObject[idxObject] = Object();
	listObject[idxObject].buildPolygon(listPoint, pointSize);
	idxObject++;
	free(listPoint);
    //////////////////////////////////////////////////////////////////////////////////////////
    pointSize = 12;
    listPoint = (Point*)malloc(pointSize*sizeof(Point));
    int xHMTL[12], yHMTL[12];
	xHMTL[0] = 234; yHMTL[0] = 235;
	xHMTL[1] = 244; yHMTL[1] = 235;
	xHMTL[2] = 244; yHMTL[2] = 238;
	xHMTL[3] = 302; yHMTL[3] = 238;
	xHMTL[4] = 302; yHMTL[4] = 250;
	xHMTL[5] = 286; yHMTL[5] = 250;
	xHMTL[6] = 286; yHMTL[6] = 253;
	xHMTL[7] = 280; yHMTL[7] = 253;
	xHMTL[8] = 280; yHMTL[8] = 250;
	xHMTL[9] = 244; yHMTL[9] = 250;
	xHMTL[10] = 244; yHMTL[10] = 253;
	xHMTL[11] = 234; yHMTL[11] = 253;
	for(int i = 0; i < pointSize; i++) {
        listPoint[i].x = xHMTL[i];
        listPoint[i].y = yHMTL[i];
	}
	listObject[idxObject] = Object();
	listObject[idxObject].buildPolygon(listPoint, pointSize);
	idxObject++;
	free(listPoint);
    //////////////////////////////////////////////////////////////////////////////////////////
    pointSize = 6;
    listPoint = (Point*)malloc(pointSize*sizeof(Point));
    int xLIXA[6], yLIXA[6];
	xLIXA[0] = 234; yLIXA[0] = 259;
	xLIXA[1] = 269; yLIXA[1] = 259;
	xLIXA[2] = 269; yLIXA[2] = 273;
	xLIXA[3] = 250; yLIXA[3] = 273;
	xLIXA[4] = 250; yLIXA[4] = 282;
	xLIXA[5] = 234; yLIXA[5] = 282;
	for(int i = 0; i < pointSize; i++) {
        listPoint[i].x = xLIXA[i];
        listPoint[i].y = yLIXA[i];
	}
	listObject[idxObject] = Object();
	listObject[idxObject].buildPolygon(listPoint, pointSize);
	idxObject++;
	free(listPoint);
    //////////////////////////////////////////////////////////////////////////////////////////
    pointSize = 4;
    listPoint = (Point*)malloc(pointSize*sizeof(Point));
    int xLIXC[4], yLIXC[4];
	xLIXC[0] = 272; yLIXC[0] = 259;
	xLIXC[1] = 304; yLIXC[1] = 259;
	xLIXC[2] = 304; yLIXC[2] = 273;
	xLIXC[3] = 272; yLIXC[3] = 273;
	for(int i = 0; i < pointSize; i++) {
        listPoint[i].x = xLIXC[i];
        listPoint[i].y = yLIXC[i];
	}
	listObject[idxObject] = Object();
	listObject[idxObject].buildPolygon(listPoint, pointSize);
	idxObject++;
	free(listPoint);
    //////////////////////////////////////////////////////////////////////////////////////////
    pointSize = 4;
    listPoint = (Point*)malloc(pointSize*sizeof(Point));
    int xKotakKecil[4], yKotakKecil[4];
	xKotakKecil[0] = 267; yKotakKecil[0] = 274;
	xKotakKecil[1] = 275; yKotakKecil[1] = 274;
	xKotakKecil[2] = 275; yKotakKecil[2] = 281;
	xKotakKecil[3] = 267; yKotakKecil[3] = 281;
	for(int i = 0; i < pointSize; i++) {
        listPoint[i].x = xKotakKecil[i];
        listPoint[i].y = yKotakKecil[i];
	}
	listObject[idxObject] = Object();
	listObject[idxObject].buildPolygon(listPoint, pointSize);
	idxObject++;
	free(listPoint);
    //////////////////////////////////////////////////////////////////////////////////////////
    pointSize = 4;
    listPoint = (Point*)malloc(pointSize*sizeof(Point));
    int xLIXB[4], yLIXB[4];
	xLIXB[0] = 272; yLIXB[0] = 281;
	xLIXB[1] = 304; yLIXB[1] = 281;
	xLIXB[2] = 304; yLIXB[2] = 295;
	xLIXB[3] = 272; yLIXB[3] = 295;
	for(int i = 0; i < pointSize; i++) {
        listPoint[i].x = xLIXB[i];
        listPoint[i].y = yLIXB[i];
	}
	listObject[idxObject] = Object();
	listObject[idxObject].buildPolygon(listPoint, pointSize);
	idxObject++;
	free(listPoint);
    //////////////////////////////////////////////////////////////////////////////////////////
    pointSize = 4;
    listPoint = (Point*)malloc(pointSize*sizeof(Point));
    int xMerahB[4], yMerahB[4];
	xMerahB[0] = 243; yMerahB[0] = 282;
	xMerahB[1] = 269; yMerahB[1] = 282;
	xMerahB[2] = 269; yMerahB[2] = 295;
	xMerahB[3] = 242; yMerahB[3] = 295;
	for(int i = 0; i < pointSize; i++) {
        listPoint[i].x = xMerahB[i];
        listPoint[i].y = yMerahB[i];
	}
	listObject[idxObject] = Object();
	listObject[idxObject].buildPolygon(listPoint, pointSize);
	idxObject++;
	free(listPoint);
    //////////////////////////////////////////////////////////////////////////////////////////
    pointSize = 4;
    listPoint = (Point*)malloc(pointSize*sizeof(Point));
    int xMerahK1[4], yMerahK1[4];
	xMerahK1[0] = 247; yMerahK1[0] = 298;
	xMerahK1[1] = 256; yMerahK1[1] = 298;
	xMerahK1[2] = 256; yMerahK1[2] = 311;
	xMerahK1[3] = 247; yMerahK1[3] = 311;
	for(int i = 0; i < pointSize; i++) {
        listPoint[i].x = xMerahK1[i];
        listPoint[i].y = yMerahK1[i];
	}
	listObject[idxObject] = Object();
	listObject[idxObject].buildPolygon(listPoint, pointSize);
	idxObject++;
	free(listPoint);
    //////////////////////////////////////////////////////////////////////////////////////////
    pointSize = 4;
    listPoint = (Point*)malloc(pointSize*sizeof(Point));
    int xMerahK2[4], yMerahK2[4];
	xMerahK2[0] = 258; yMerahK2[0] = 298;
	xMerahK2[1] = 268; yMerahK2[1] = 298;
	xMerahK2[2] = 268; yMerahK2[2] = 311;
	xMerahK2[3] = 258; yMerahK2[3] = 311;
	for(int i = 0; i < pointSize; i++) {
        listPoint[i].x = xMerahK2[i];
        listPoint[i].y = yMerahK2[i];
	}
	listObject[idxObject] = Object();
	listObject[idxObject].buildPolygon(listPoint, pointSize);
	idxObject++;
	free(listPoint);
    //////////////////////////////////////////////////////////////////////////////////////////
    pointSize = 4;
    listPoint = (Point*)malloc(pointSize*sizeof(Point));
    int xMerahK3[4], yMerahK3[4];
	xMerahK3[0] = 256; yMerahK3[0] = 312;
	xMerahK3[1] = 268; yMerahK3[1] = 312;
	xMerahK3[2] = 268; yMerahK3[2] = 320;
	xMerahK3[3] = 256; yMerahK3[3] = 320;
	for(int i = 0; i < pointSize; i++) {
        listPoint[i].x = xMerahK3[i];
        listPoint[i].y = yMerahK3[i];
	}
	listObject[idxObject] = Object();
	listObject[idxObject].buildPolygon(listPoint, pointSize);
	idxObject++;
	free(listPoint);
    //////////////////////////////////////////////////////////////////////////////////////////
    pointSize = 7;
    listPoint = (Point*)malloc(pointSize*sizeof(Point));
    int xMerah[7], yMerah[7];
	xMerah[0] = 270; yMerah[0] = 300;
	xMerah[1] = 280; yMerah[1] = 300;
	xMerah[2] = 280; yMerah[2] = 306;
	xMerah[3] = 282; yMerah[3] = 306;
	xMerah[4] = 282; yMerah[4] = 314;
	xMerah[5] = 284; yMerah[5] = 314;
	xMerah[6] = 270; yMerah[6] = 325;
	for(int i = 0; i < pointSize; i++) {
        listPoint[i].x = xMerah[i];
        listPoint[i].y = yMerah[i];
	}
	listObject[idxObject] = Object();
	listObject[idxObject].buildPolygon(listPoint, pointSize);
	idxObject++;
	free(listPoint);
	//////////////////////////////////////////////////////////////////////////////////////////
    pointSize = 4;
    listPoint = (Point*)malloc(pointSize*sizeof(Point));
    int xUMH[4], yUMH[4];
	xUMH[0] = 272; yUMH[0] = 177;
	xUMH[1] = 318; yUMH[1] = 177;
	xUMH[2] = 318; yUMH[2] = 164;
	xUMH[3] = 272; yUMH[3] = 164;
	for(int i = 0; i < pointSize; i++) {
        listPoint[i].x = xUMH[i];
        listPoint[i].y = yUMH[i];
	}
	listObject[idxObject] = Object();
	listObject[idxObject].buildPolygon(listPoint, pointSize);
	idxObject++;
	free(listPoint);
	//////////////////////////////////////////////////////////////////////////////////////////
    pointSize = 8;
    listPoint = (Point*)malloc(pointSize*sizeof(Point));
    int xGKUTim[8], yGKUTim[8];
	xGKUTim[0] = 284; yGKUTim[0] = 180;
	xGKUTim[1] = 318; yGKUTim[1] = 180;
	xGKUTim[2] = 318; yGKUTim[2] = 195;
	xGKUTim[3] = 312; yGKUTim[3] = 196;
	xGKUTim[4] = 312; yGKUTim[4] = 192;
	xGKUTim[5] = 298; yGKUTim[5] = 192;
	xGKUTim[6] = 298; yGKUTim[6] = 196;
	xGKUTim[7] = 284; yGKUTim[7] = 196;
	for(int i = 0; i < pointSize; i++) {
        listPoint[i].x = xGKUTim[i];
        listPoint[i].y = yGKUTim[i];
	}
	listObject[idxObject] = Object();
	listObject[idxObject].buildPolygon(listPoint, pointSize);
	idxObject++;
	free(listPoint);
	//////////////////////////////////////////////////////////////////////////////////////////
    pointSize = 4;
    listPoint = (Point*)malloc(pointSize*sizeof(Point));
    int xTGeo[4], yTGeo[4];
	xTGeo[0] = 308; yTGeo[0] = 197;
	xTGeo[1] = 321; yTGeo[1] = 197;
	xTGeo[2] = 321; yTGeo[2] = 212;
	xTGeo[3] = 308; yTGeo[3] = 212;
	for(int i = 0; i < pointSize; i++) {
        listPoint[i].x = xTGeo[i];
        listPoint[i].y = yTGeo[i];
	}
	listObject[idxObject] = Object();
	listObject[idxObject].buildPolygon(listPoint, pointSize);
	idxObject++;
	free(listPoint);
	//////////////////////////////////////////////////////////////////////////////////////////
    pointSize = 4;
    listPoint = (Point*)malloc(pointSize*sizeof(Point));
    int xM[4], yM[4];
	xM[0] = 308; yM[0] = 212;
	xM[1] = 316; yM[1] = 212;
	xM[2] = 316; yM[2] = 225;
	xM[3] = 308; yM[3] = 225;
	for(int i = 0; i < pointSize; i++) {
        listPoint[i].x = xM[i];
        listPoint[i].y = yM[i];
	}
	listObject[idxObject] = Object();
	listObject[idxObject].buildPolygon(listPoint, pointSize);
	idxObject++;
	free(listPoint);
	//////////////////////////////////////////////////////////////////////////////////////////
    pointSize = 12;
    listPoint = (Point*)malloc(pointSize*sizeof(Point));
    int xM2[12], yM2[12];
	xM2[0] = 310; yM2[0] = 228;
	xM2[1] = 316; yM2[1] = 228;
	xM2[2] = 316; yM2[2] = 225;
	xM2[3] = 321; yM2[3] = 225;
	xM2[4] = 321; yM2[4] = 230;
	xM2[5] = 318; yM2[5] = 230;
	xM2[6] = 318; yM2[6] = 233;
	xM2[7] = 321; yM2[7] = 233;
	xM2[8] = 321; yM2[8] = 236;
	xM2[9] = 316; yM2[9] = 236;
	xM2[10] = 316; yM2[10] = 234;
	xM2[11] = 310; yM2[11] = 233;
	for(int i = 0; i < pointSize; i++) {
        listPoint[i].x = xM2[i];
        listPoint[i].y = yM2[i];
	}
	listObject[idxObject] = Object();
	listObject[idxObject].buildPolygon(listPoint, pointSize);
	idxObject++;
	free(listPoint);
	//////////////////////////////////////////////////////////////////////////////////////////
    pointSize = 4;
    listPoint = (Point*)malloc(pointSize*sizeof(Point));
    int xAP[4], yAP[4];
	xAP[0] = 268; yAP[0] = 200;
	xAP[1] = 297; yAP[1] = 200;
	xAP[2] = 297; yAP[2] = 225;
	xAP[3] = 268; yAP[3] = 225;
	for(int i = 0; i < pointSize; i++) {
        listPoint[i].x = xAP[i];
        listPoint[i].y = yAP[i];
	}
	listObject[idxObject] = Object();
	listObject[idxObject].buildPolygon(listPoint, pointSize);
	idxObject++;
	free(listPoint);
	//////////////////////////////////////////////////////////////////////////////////////////
    pointSize = 8;
    listPoint = (Point*)malloc(pointSize*sizeof(Point));
    int xAP1[8], yAP1[8];
	xAP1[0] = 268; yAP1[0] = 183;
	xAP1[1] = 277; yAP1[1] = 183;
	xAP1[2] = 277; yAP1[2] = 177;
	xAP1[3] = 281; yAP1[3] = 177;
	xAP1[4] = 281; yAP1[4] = 182;
	xAP1[5] = 284; yAP1[5] = 183;
	xAP1[6] = 284; yAP1[6] = 195;
	xAP1[7] = 267; yAP1[7] = 196;
	for(int i = 0; i < pointSize; i++) {
        listPoint[i].x = xAP1[i];
        listPoint[i].y = yAP1[i];
	}
	listObject[idxObject] = Object();
	listObject[idxObject].buildPolygon(listPoint, pointSize);
	idxObject++;
	free(listPoint);
	//////////////////////////////////////////////////////////////////////////////////////////
    pointSize = 4;
    listPoint = (Point*)malloc(pointSize*sizeof(Point));
    int xLapBas[4], yLapBas[4];
	xLapBas[0] = 146; yLapBas[0] = 256;
	xLapBas[1] = 170; yLapBas[1] = 256;
	xLapBas[2] = 170; yLapBas[2] = 287;
	xLapBas[3] = 146; yLapBas[3] = 287;
	for(int i = 0; i < pointSize; i++) {
        listPoint[i].x = xLapBas[i];
        listPoint[i].y = yLapBas[i];
	}
	listObject[idxObject] = Object();
	listObject[idxObject].buildPolygon(listPoint, pointSize);
	idxObject++;
	free(listPoint);
	//////////////////////////////////////////////////////////////////////////////////////////
    pointSize = 4;
    listPoint = (Point*)malloc(pointSize*sizeof(Point));
    int xGedung[4], yGedung[4];
	xGedung[0] = 147; yGedung[0] = 288;
	xGedung[1] = 127; yGedung[1] = 288;
	xGedung[2] = 127; yGedung[2] = 304;
	xGedung[3] = 147; yGedung[3] = 304;
	for(int i = 0; i < pointSize; i++) {
        listPoint[i].x = xGedung[i];
        listPoint[i].y = yGedung[i];
	}
	listObject[idxObject] = Object();
	listObject[idxObject].buildPolygon(listPoint, pointSize);
	idxObject++;
	free(listPoint);
	//////////////////////////////////////////////////////////////////////////////////////////
    pointSize = 4;
    listPoint = (Point*)malloc(pointSize*sizeof(Point));
    int xCCBar[4], yCCBar[4];
	xCCBar[0] = 143; yCCBar[0] = 240;
	xCCBar[1] = 174; yCCBar[1] = 240;
	xCCBar[2] = 174; yCCBar[2] = 251;
	xCCBar[3] = 143; yCCBar[3] = 251;
	for(int i = 0; i < pointSize; i++) {
        listPoint[i].x = xCCBar[i];
        listPoint[i].y = yCCBar[i];
	}
	listObject[idxObject] = Object();
	listObject[idxObject].buildPolygon(listPoint, pointSize);
	idxObject++;
	free(listPoint);
    //////////////////////////////////////////////////////////////////////////////////////////
	pointSize = 14;
    listPoint = (Point*)malloc(pointSize*sizeof(Point));
    int xTekSipil[14], yTekSipil[14];
	xTekSipil[0] = 62; yTekSipil[0] = 272;
	xTekSipil[1] = 86; yTekSipil[1] = 272;
	xTekSipil[2] = 86; yTekSipil[2] = 281;
	xTekSipil[3] = 74; yTekSipil[3] = 281;
	xTekSipil[4] = 74; yTekSipil[4] = 285;
	xTekSipil[5] = 111; yTekSipil[5] = 285;
	xTekSipil[6] = 111; yTekSipil[6] = 280;
	xTekSipil[7] = 125; yTekSipil[7] = 280;
	xTekSipil[8] = 125; yTekSipil[8] = 308;
	xTekSipil[9] = 70; yTekSipil[9] = 308;
	xTekSipil[10] = 70; yTekSipil[10] = 299;
	xTekSipil[11] = 98; yTekSipil[11] = 299;
	xTekSipil[12] = 98; yTekSipil[12] = 294;
	xTekSipil[13] = 62; yTekSipil[13] = 294;
	for(int i = 0; i < pointSize; i++) {
        listPoint[i].x = xTekSipil[i];
        listPoint[i].y = yTekSipil[i];
	}
	listObject[idxObject] = Object();
	listObject[idxObject].buildPolygon(listPoint, pointSize);
	idxObject++;
	free(listPoint);
	//////////////////////////////////////////////////////////////////////////////////////////
	pointSize = 8;
    listPoint = (Point*)malloc(pointSize*sizeof(Point));
    int xHimafi[8], yHimafi[8];
	xHimafi[0] = 60; yHimafi[0] = 233;
	xHimafi[1] = 125; yHimafi[1] = 233;
	xHimafi[2] = 125; yHimafi[2] = 277;
	xHimafi[3] = 89; yHimafi[3] = 277;
	xHimafi[4] = 89; yHimafi[4] = 269;
	xHimafi[5] = 118; yHimafi[5] = 269;
	xHimafi[6] = 118; yHimafi[6] = 266;
	xHimafi[7] = 60; yHimafi[7] = 266;

	for(int i = 0; i < pointSize; i++) {
        listPoint[i].x = xHimafi[i];
        listPoint[i].y = yHimafi[i];
	}
	listObject[idxObject] = Object();
	listObject[idxObject].buildPolygon(listPoint, pointSize);
	idxObject++;
	free(listPoint);
    //////////////////////////////////////////////////////////////////////////////////////////
	pointSize = 4;
    listPoint = (Point*)malloc(pointSize*sizeof(Point));
    int xA[4], yA[4];
	xA[0] = 30; yA[0] = 111;
	xA[1] = 78; yA[1] = 111;
	xA[2] = 78; yA[2] = 138;
	xA[3] = 30; yA[3] = 138;
	for(int i = 0; i < pointSize; i++) {
        listPoint[i].x = xA[i];
        listPoint[i].y = yA[i];
	}
	listObject[idxObject] = Object();
	listObject[idxObject].buildPolygon(listPoint, pointSize);
	idxObject++;
	free(listPoint);
	//////////////////////////////////////////////////////////////////////////////////////////
	pointSize = 4;
    listPoint = (Point*)malloc(pointSize*sizeof(Point));
    int xB[4], yB[4];
	xB[0] = 30; yB[0] = 140;
	xB[1] = 55; yB[1] = 140;
	xB[2] = 55; yB[2] = 153;
	xB[3] = 30; yB[3] = 153;
	for(int i = 0; i < pointSize; i++) {
        listPoint[i].x = xB[i];
        listPoint[i].y = yB[i];
	}
	listObject[idxObject] = Object();
	listObject[idxObject].buildPolygon(listPoint, pointSize);
	idxObject++;
	free(listPoint);
	//////////////////////////////////////////////////////////////////////////////////////////
	pointSize = 12;
    listPoint = (Point*)malloc(pointSize*sizeof(Point));
    int xC[12], yC[12];
	xC[0] = 30; yC[0] = 155;
	xC[1] = 55; yC[1] = 155;
	xC[2] = 55; yC[2] = 158;
	xC[3] = 65; yC[3] = 158;
	xC[4] = 65; yC[4] = 153;
	xC[5] = 75; yC[5] = 153;
	xC[6] = 75; yC[6] = 169;
	xC[7] = 65; yC[7] = 169;
	xC[8] = 65; yC[8] = 166;
	xC[9] = 55; yC[9] = 166;
	xC[10] = 55; yC[10] = 169;
	xC[11] = 30; yC[11] = 169;
	for(int i = 0; i < pointSize; i++) {
        listPoint[i].x = xC[i];
        listPoint[i].y = yC[i];
	}
	listObject[idxObject] = Object();
	listObject[idxObject].buildPolygon(listPoint, pointSize);
	idxObject++;
	free(listPoint);
	//////////////////////////////////////////////////////////////////////////////////////////
	pointSize = 11;
    listPoint = (Point*)malloc(pointSize*sizeof(Point));
    int xD[11], yD[11];
	xD[0] = 46; yD[0] = 94;
	xD[1] = 71; yD[1] = 94;
	xD[2] = 71; yD[2] = 99;
	xD[3] = 73; yD[3] = 99;
	xD[4] = 73; yD[4] = 103;
	xD[5] = 71; yD[5] = 103;
	xD[6] = 71; yD[6] = 108;
	xD[7] = 47; yD[7] = 108;
	xD[8] = 46; yD[8] = 99;
	xD[9] = 33; yD[9] = 99;
	xD[10] = 33; yD[10] = 97;
	for(int i = 0; i < pointSize; i++) {
        listPoint[i].x = xD[i];
        listPoint[i].y = yD[i];
	}
	listObject[idxObject] = Object();
	listObject[idxObject].buildPolygon(listPoint, pointSize);
	idxObject++;
	free(listPoint);
}

void drawAllObject() {
    if (listA) {
        for(int i = 0; i < nObject/2; i++) {
            int x0 = listObject[i].position.x;
            int y0 = listObject[i].position.y;
            for (int j = 0; j < listObject[i].nLines; j++) {
                Point p1 = listObject[i].lines[j].p1;
                Point p2 = listObject[i].lines[j].p2;
                drawWhiteLine(x0 + p1.x, y0 + p1.y, x0 + p2.x, y0 + p2.y);
            }
        }
    }
    if (listB){
        for(int i = nObject/2; i < nObject; i++) {
            int x0 = listObject[i].position.x;
            int y0 = listObject[i].position.y;
            for (int j = 0; j < listObject[i].nLines; j++) {
                Point p1 = listObject[i].lines[j].p1;
                Point p2 = listObject[i].lines[j].p2;
                drawWhiteLine(x0 + p1.x, y0 + p1.y, x0 + p2.x, y0 + p2.y);
            }
        }
    }
}



//CLipper Constants
bool isClipping = false;
int xClipper = 100;
int yClipper = 100;
int pClipper = 100;
int lClipper = 100;

void clip(int xClipper, int yClipper, int pClipper, int lClipper) {
	if (isClipping) {
		for (int i = 0; i < WIDTH; ++i)
		{
			for (int j = 0; j < HEIGHT; ++j)
			{
				if ((abs(xClipper - i) > pClipper) || (abs(yClipper - j) > lClipper)) {
					redPixelMatrix[i][j] = 0;
					greenPixelMatrix[i][j] = 0;
					bluePixelMatrix[i][j] = 0;
				}
			}
		}
		drawWhiteLine(xClipper+pClipper, yClipper+lClipper, xClipper-pClipper, yClipper+lClipper);
		drawWhiteLine(xClipper+pClipper, yClipper+lClipper, xClipper+pClipper, yClipper-lClipper);
		drawWhiteLine(xClipper+pClipper, yClipper-lClipper, xClipper-pClipper, yClipper-lClipper);
		drawWhiteLine(xClipper-pClipper, yClipper+lClipper, xClipper-pClipper, yClipper-lClipper);
	}
}

bool isClipped(Line l, Point pos) {
    double my = (double)(l.p1.y - l.p2.y)/(l.p1.x - l.p2.x);
    double cy = (double)(pos.y+l.p1.y) - my;
    double mx = (double)(l.p1.x - l.p2.x)/(l.p1.y - l.p2.y);
    double cx = (double)(pos.x+l.p1.x) - mx;
    int x1 = 0;
    int x2 = WIDTH;
    int y1 = 0;
    int y2 = HEIGHT;
    if((my*x1+cy) <= y2 && (my*x1+cy) >= y1
        || ((my*x2+cy) <= y2 && (my*x2+cy) >= y1)
        || ((mx*y1+cx) <= x2 && (mx*y1+cx) >= x1)
        || ((mx*y2+cx) <= x2 && (mx*y2+cx) >= x1)) {
        if (((pos.y+l.p1.y < y1 && pos.y+l.p2.y < y1)
            ||(pos.y+l.p1.y > y2 && pos.y+l.p2.y > y2)
            || (pos.x+l.p1.x < x1 && pos.x+l.p2.x < x1)
            || (pos.x+l.p1.x > x2 && pos.x+l.p2.x > x2))) {
            return false;
        } else {
            return true;
        }
    } else {
        return false;
    }
}
void drawClipObject(Object o){
    Point pos = o.position;
    for (int i = 0; i < o.nLines; i++) {
        if (isClipped(o.lines[i], o.position)) {
            Point p1 = o.lines[i].p1;
            Point p2 = o.lines[i].p2;
            drawWhiteLineZoom(pos.x + p1.x, pos.y + p1.y, pos.x + p2.x, pos.y + p2.y);
        }
    }
}
void drawClipAllObject() {
    double ratioX = (double)WIDTH/(2*lClipper-1);
    double ratioY = (double)HEIGHT/(2*pClipper-1);
    if (listA) {
        for(int i = 0; i < nObject/2; i++){
            drawClipObject(listObject[i].translate(WIDTH/2-xClipper, HEIGHT/2-yClipper).zoom(ratioX,ratioY,Point(WIDTH/2,HEIGHT/2)));
        }
    }
    if (listB) {
        for(int i = nObject/2; i < nObject; i++){
            drawClipObject(listObject[i].translate(WIDTH/2-xClipper, HEIGHT/2-yClipper).zoom(ratioX,ratioY,Point(WIDTH/2,HEIGHT/2)));
        }
    }
}

int detectKeyStroke() {
    //deteksi adanya keyboard yang ditekan.
    //0 jika tidak, >=1 jika iya
	static bool flag = false;
	static const int STDIN = 0;

	if (!flag) {
        //Memakai termios untuk mematikan line buffering
		struct termios T;

		tcgetattr(STDIN, &T);
		T.c_lflag &= ~ICANON;
		tcsetattr(STDIN, TCSANOW, &T);
		setbuf(stdin, NULL);
		flag = true;
	}

	int NByte;
    ioctl(STDIN, FIONREAD, &NByte);  // STDIN = 0

    return NByte;
}

void DrawToScreen(){
    /* prosedur yang menggambar ke layar dari matriks RGB (harusnya rata tengah)*/
	long int location = 0;
	int x , y;
	for (y = vinfo.yres/2 - WIDTH/2; y < WIDTH + vinfo.yres/2 - WIDTH/2; y++)
		for (x = vinfo.xres/2 - HEIGHT/2; x < HEIGHT + vinfo.xres/2 - HEIGHT/2; x++) {
			location = (x+vinfo.xoffset) * (vinfo.bits_per_pixel/8) + (y+vinfo.yoffset) * finfo.line_length;
			if (vinfo.bits_per_pixel == 32) {
        //4byte
        *(fbp + location) = bluePixelMatrix[y - vinfo.yres/2 + WIDTH/2][x - vinfo.xres/2 + HEIGHT/2];        // Some blue
        *(fbp + location + 1) = greenPixelMatrix[y - vinfo.yres/2 + WIDTH/2][x - vinfo.xres/2 + HEIGHT/2];     // A little green
        *(fbp + location + 2) = redPixelMatrix[y - vinfo.yres/2 + WIDTH/2][x - vinfo.xres/2 + HEIGHT/2];    // A lot of red
        *(fbp + location + 3) = 0;      // No transparency
        //location += 4;
      } else  { //assume 16bpp
        	int b = 0;
          int g = 100;     // A little green
          int r = 0;    // A lot of red
          unsigned short int t = r<<11 | g << 5 | b;
          *((unsigned short int*)(fbp + location)) = t;
      }
    }
}

int redTempMatriks[WIDTH][HEIGHT*2];
int greenTempMatriks[WIDTH][HEIGHT*2];
int blueTempMatriks[WIDTH][HEIGHT*2];

void DrawMapAndMiniMapToScreen(){
		/* menggambar mini map dan map yang bisa zoom in/out */
		/* mini map dikiri dan satunya dikanan */
		/* harusnya rata tengah */
	  // buat temporer matriks

		long int location = 0;
		int x , y;
		int HEIGHT2 = HEIGHT*2;
		for(int i=0;i<WIDTH;i++){
			for(int j=0;j<HEIGHT2;j++){
					if(j<HEIGHT){
						redTempMatriks[i][j] = redPixelMatrix[i][j];
						greenTempMatriks[i][j] = greenPixelMatrix[i][j];
						blueTempMatriks[i][j] = bluePixelMatrix[i][j];
					}else{

						redTempMatriks[i][j] = redPixelMatrixZoomMap[i][j - HEIGHT];
						greenTempMatriks[i][j] = greenPixelMatrixZoomMap[i][j - HEIGHT];
						blueTempMatriks[i][j] = bluePixelMatrixZoomMap[i][j - HEIGHT];
					}
			}
		}

		for (y = vinfo.yres/2 - WIDTH/2; y < WIDTH + vinfo.yres/2 - WIDTH/2; y++)
			for (x = vinfo.xres/2 - HEIGHT2/2; x < HEIGHT2 + vinfo.xres/2 - HEIGHT2/2; x++) {
				location = (x+vinfo.xoffset) * (vinfo.bits_per_pixel/8) + (y+vinfo.yoffset) * finfo.line_length;
				if (vinfo.bits_per_pixel == 32) {
	        //4byte
	        *(fbp + location) = blueTempMatriks[y - vinfo.yres/2 + WIDTH/2][x - vinfo.xres/2 + HEIGHT2/2];        // Some blue
	        *(fbp + location + 1) = greenTempMatriks[y - vinfo.yres/2 + WIDTH/2][x - vinfo.xres/2 + HEIGHT2/2];     // A little green
	        *(fbp + location + 2) = redTempMatriks[y - vinfo.yres/2 + WIDTH/2][x - vinfo.xres/2 + HEIGHT2/2];    // A lot of red
	        *(fbp + location + 3) = 0;      // No transparency
	        //location += 4;
	      } else  { //assume 16bpp
	        	int b = 0;
	          int g = 100;     // A little green
	          int r = 0;    // A lot of red
	          unsigned short int t = r<<11 | g << 5 | b;
	          *((unsigned short int*)(fbp + location)) = t;
	      }
	    }

}

    void drawFrame() {
    	drawWhiteLine(1, 1, 1, HEIGHT-1);
    	drawWhiteLine(1, HEIGHT-1, WIDTH-1, HEIGHT-1);
    	drawWhiteLine(WIDTH-1, HEIGHT-1,WIDTH-1, 1);
    	drawWhiteLine(WIDTH-1, 1, 1, 1);

        drawWhiteLine2(1, 1, 1, HEIGHT-1);
    	drawWhiteLine2(1, HEIGHT-1, WIDTH-1, HEIGHT-1);
    	drawWhiteLine2(WIDTH-1, HEIGHT-1,WIDTH-1, 1);
    	drawWhiteLine2(WIDTH-1, 1, 1, 1);
    }

		void drawKeyClipping(){
    	while(true){
    		if(!detectKeyStroke()) {
					char KeyPressed = getchar();
    			if (KeyPressed=='j' || KeyPressed=='l' || KeyPressed=='i' || KeyPressed=='k' || KeyPressed=='z' || KeyPressed=='s' || KeyPressed=='q' || KeyPressed=='w') { //Listener Clipper
    				//IsClip

    				//Clipper gerak kiri kanan atas bawah
    				if (KeyPressed == 'j') {
    					yClipper -= 20;
    				}
    				if (KeyPressed == 'l') {
    					yClipper += 20;
    				}
    				if (KeyPressed == 'i') {
    					xClipper -= 20;
    				}
    				if (KeyPressed == 'k') {
    					xClipper += 20;
    				}
    				if (KeyPressed == 'z') {
                        if (pClipper < 200) {
                            pClipper += 10;
                            lClipper += 10;
                        }
    				}
    				if (KeyPressed == 's') {
                        if (pClipper > 50) {
                            pClipper -= 10;
                            lClipper -= 10;
                        }
    				}
    				if (KeyPressed == 'q') {
                        listA = !listA;
    				}
    				if (KeyPressed == 'w') {
                        listB = !listB;
    				}

    				//Penanganan x diluar frame buffer
    				if (xClipper + pClipper > WIDTH) {
    					xClipper = WIDTH - pClipper - 1;
    				}
    				if (xClipper - pClipper < 0) {
    					xClipper = pClipper + 1;
    				}
    				//Penanganan y diluar frame buffer
    				if (yClipper + lClipper > HEIGHT) {
    					yClipper = HEIGHT - lClipper - 1;
    				}
    				if (yClipper - lClipper < 0) {
    					yClipper = lClipper + 1;
    				}
    			}
    		}
    	}

    }


int main() {
	clearMatrix();

	int fbfd = 0;
	long int screensize = 0;
	exploded = false;

	fbfd = open("/dev/fb0", O_RDWR);
	if (fbfd == -1) {
		perror("Error: cannot open framebuffer device");
		exit(1);
	}
    // Get fixed screen information
	if (ioctl(fbfd, FBIOGET_FSCREENINFO, &finfo) == -1) {
		perror("Error reading fixed information");
		exit(2);
	}
    // Get variable screen information
	if (ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo) == -1) {
		perror("Error reading variable information");
		exit(3);
	}

    // mendapat screensize layar monitor
	screensize = vinfo.xres * vinfo.yres * vinfo.bits_per_pixel / 8;
	printf("%d x %d\n",vinfo.xres,vinfo.yres);
    // Map the device to memory
	fbp = (char *)mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, (off_t)0);

    //display merge center
    // Menulis ke layar tengah file
    //Gambar trapesium


	thread thread1(&drawKeyClipping);
	char KeyPressed;
	isClipping = true;
	clearMatrix();
	makeWorld();
	clip(xClipper, yClipper, pClipper, lClipper);
	do{
		clearMatrix();
		clip(xClipper, yClipper, pClipper, lClipper);
		drawFrame();
		drawAllObject();
		drawClipAllObject();
        DrawMapAndMiniMapToScreen();
		usleep(40000);
	} while (KeyPressed!='C' && !exploded);
	thread1.detach();
	munmap(fbp, screensize);
	close(fbfd);
	return 0;
}
