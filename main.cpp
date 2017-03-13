#include <iostream>
#include <mutex>
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

using namespace std;

#define WIDTH 1300
#define HEIGHT 700
#define lookSize 400

typedef struct{
    int x;
    int y;
} Point;

struct fb_var_screeninfo vinfo;
struct fb_fix_screeninfo finfo;
char *fbp = 0;

int redPixelMatrix[WIDTH][HEIGHT];
int greenPixelMatrix[WIDTH][HEIGHT];
int bluePixelMatrix[WIDTH][HEIGHT];
int posX;
int posY;
int lastCorrectState = 's';
bool exploded = false;
bool destroyed[WIDTH][HEIGHT];

int redClipperMatrix[lookSize][lookSize];
int greenClipperMatrix[lookSize][lookSize];
int blueClipperMatrix[lookSize][lookSize];

int x_god;
int y_god;

int size_god;

unsigned char* BMP;
int BMP_width, BMP_height;

bool pohon = true;
bool jalan = true;

void initDestroyed() {
    for (int i = 0; i < WIDTH; ++i)
    {
        for (int j = 0; j < HEIGHT; ++j)
        {
            destroyed[i][j] = false;
        }
    }
}

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
}

void clearClipperMatrix() {
    for (int i = 0; i < 400; ++i)
    {
        for (int j = 0; j < 400; ++j)
        {
            redClipperMatrix[i][j] = 0;
            greenClipperMatrix[i][j] = 0;
            blueClipperMatrix[i][j] = 0;
        }
    }
}

void drawWhitePoint(int x1, int y1) {
    if (x1 < 0 || x1 >= WIDTH || y1 < 0 || y1 >= HEIGHT) return;
    redPixelMatrix[x1][y1] = 255;
    greenPixelMatrix[x1][y1] = 255;
    bluePixelMatrix[x1][y1] = 255;
}

void drawYellowPoint(int x1,int y1){
    if (x1 < 0 || x1 >= WIDTH || y1 < 0 || y1 >= HEIGHT) return;
    redPixelMatrix[x1][y1] = 255;
    greenPixelMatrix[x1][y1] = 255;
    bluePixelMatrix[x1][y1] = 0;
}

void drawBlackPoint(int x1,int y1){
    if (x1 < 0 || x1 >= WIDTH || y1 < 0 || y1 >= HEIGHT) return;
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


void drawYellowLine(int x1, int y1, int x2, int y2) {
    //Than kode lu gua benerin dikit di sini, harusnya ngk usah pake absolut
    int deltaX = x2 - x1;
    int deltaY = y2 - y1;
    int ix = deltaX > 0 ? 1 : -1;
    int iy = deltaY > 0 ? 1 : -1;
    deltaX = abs(deltaX);
    deltaY = abs(deltaY);

    int x = x1;
    int y = y1;

    drawYellowPoint(x,y);

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

            drawYellowPoint(x, y);
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

            drawYellowPoint(x, y);
        }
    }
}

void floodFill(int x,int y,int redBatas,int greenBatas,int blueBatas,int redColor,int greenColor,int blueColor){
    if((x>=0 && x<WIDTH) && (y>=0 && y<HEIGHT)){
        if(!((redPixelMatrix[x][y]==redBatas && greenPixelMatrix[x][y]==greenBatas && bluePixelMatrix[x][y]==blueBatas) ||
            (redPixelMatrix[x][y]==redColor && greenPixelMatrix[x][y]==greenColor && bluePixelMatrix[x][y]==blueColor))){
            redPixelMatrix[x][y] = redColor;
            greenPixelMatrix[x][y] = greenColor;
            bluePixelMatrix[x][y] = blueColor;
            floodFill(x,y+1,redBatas,greenBatas,blueBatas,redColor,greenColor,blueColor);
            floodFill(x+1,y,redBatas,greenBatas,blueBatas,redColor,greenColor,blueColor);
            floodFill(x,y-1,redBatas,greenBatas,blueBatas,redColor,greenColor,blueColor);
            floodFill(x-1,y,redBatas,greenBatas,blueBatas,redColor,greenColor,blueColor);
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


int screen_size;
void DrawToScreen(){
    /* prosedure yang menggambar ke layar dari matriks RGB (harusnya rata tengah)*/
    long int location = 0;
    int x , y;
    long int bmp_loc = 0;
    for (y = 0; y < HEIGHT; y++)
        for (x = 0; x < WIDTH; x++) {
            location = (x+vinfo.xoffset) * (vinfo.bits_per_pixel/8) + (y+vinfo.yoffset) * finfo.line_length;
            if (vinfo.bits_per_pixel == 32) {
                //4byte
                // printf("%ld %d\n", location,  screen_size);
                if (location < screen_size && location > 0) {
                    *(fbp + location) = bluePixelMatrix[x][y];        // Some blue
                    *(fbp + location + 1) = greenPixelMatrix[x][y];     // A little green
                    *(fbp + location + 2) = redPixelMatrix[x][y];    // A lot of red
                    *(fbp + location + 3) = 0;      // No transparency
                }
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



//Algoritma translasi point
//x1 dan y1 adalah titik awal
//dx dan dy adalah jarak translasi
//Mengembalikan point hasil translasi
Point pointTranslation(int x1, int y1, int x2, int y2, int dx, int dy){
	Point retval;
	retval.x=x1+dx;
	retval.x=x1+dy;
	return retval;
}

//Algoritma rotasi dengan x1 dan y1 sebagai titik pivot
//Mengembalikan Point, yaitu titik baru hasil rotasi
//Algoritma sama dengan prosedur lineRotation
//Point hasil return bisa digunakan untuk drawLine ataupun drawPoint
Point lineRotate(int x1, int y1, int x2, int y2, double derajatRotasi){
	double degree;
	//jarak dari origin
	int xdiff=x1;
	int ydiff=y1;
	//mengubah sudut menjadi representasi yang sesuai
	degree=(22*derajatRotasi)/(180*7);

	x2=x2-xdiff;
	y2=y2-ydiff;

	//Titik akhir setelah rotasi
	int x3,y3;
	x3=((x2*cos(degree))-(y2*sin(degree)))+xdiff;
	y3=((x2*sin(degree))+(y2*cos(degree)))+ydiff;
	Point retval;
	retval.x=x3;
	retval.y=y3;
	return retval;
}


//x1 dan y1 adalah titik pivot rotasi, alias titik yang akan diam saja
//x2 dan y2 adalah titik yang dirotasi
//Untuk sementara, algo dibuat untuk garis merah (menyesuaikan dengan
//Algo yang ada sebelumnya)
void lineRotation(int x1, int y1, int x2, int y2, double derajatRotasi){
	double degree;
	//jarak dari origin
	int xdiff=x1;
	int ydiff=y1;
	//mengubah sudut menjadi representasi yang sesuai
	degree=(22*derajatRotasi)/(180*7);

	x2=x2-xdiff;
	y2=y2-ydiff;

	//Titik akhir setelah rotasi
	int x3,y3;
	x3=((x2*cos(degree))-(y2*sin(degree)))+xdiff;
	y3=((x2*sin(degree))+(y2*cos(degree)))+ydiff;
	drawWhiteLine(x1,y1,x3,y3);
}

//drawWhiteJalan(int x1, int y1, int x2, int y2) {}

void DrawJalan2(int x, int y) {
    drawWhiteLine(46,404,43,414);
    drawWhiteLine(44,404,41,414);
    drawWhiteLine(46,404,9,280);
    drawWhiteLine(43,404,7,280);        
    drawWhiteLine(45,407,151,377);
    drawWhiteLine(45,403,151,373);
    drawWhiteLine(151,376,148,414);
    drawWhiteLine(151,376,181,372);
    drawWhiteLine(181,373,287,403);
    drawWhiteLine(181,371,287,401);
    drawWhiteLine(288,402,288,414);
    drawWhiteLine(285,402,285,414);
    drawWhiteLine(287,404,356,404);
    drawWhiteLine(287,401,356,401);

    //itb kanan
    drawWhiteLine(306,231,308,297);
    drawWhiteLine(308,297,292,300);
    drawWhiteLine(292,300,291,335);
    drawWhiteLine(291,335,287,402);
    drawWhiteLine(266,231,266,12);
    drawWhiteLine(266,50,341,56);
    drawWhiteLine(266,111,326,116);
    drawWhiteLine(266,132,326,137);


    //itb tengah
    drawWhiteLine(184,372,184,335);
    drawWhiteLine(178,372,178,335);
    drawWhiteLine(181,337,71,337);
    drawWhiteLine(181,333,71,333);
    drawWhiteLine(184,335,184,231);
    drawWhiteLine(178,335,178,231);
    drawWhiteLine(181,233,58,233);
    drawWhiteLine(181,230,58,230);

    drawWhiteLine(58,231,23,231);
    drawWhiteLine(181,231,306,231);
    drawWhiteLine(181,335,291,335);
    drawWhiteLine(181,231,181,160);
    drawWhiteLine(181,160,181,83);
    drawWhiteLine(181,29,181,16);
    drawWhiteLine(80,16,181,16);
    drawWhiteLine(181,16,219,1);


    //itb kiri
    drawWhiteLine(22,231,22,294);
    drawWhiteLine(24,231,24,294);
    drawWhiteLine(23,293,44,329);
    drawWhiteLine(23,295,44,331);
    drawWhiteLine(44,329,71,334);
    drawWhiteLine(44,331,71,336);
    drawWhiteLine(57,231,57,315);
    drawWhiteLine(59,231,59,315);
    drawWhiteLine(57,315,70,335);
    drawWhiteLine(59,315,72,335);
    drawWhiteLine(21,231,23,173);
    drawWhiteLine(25,231,27,173);
    drawWhiteLine(25,171,82,171);
    drawWhiteLine(25,175,82,175);
    drawWhiteLine(82,172,95,172);
    drawWhiteLine(82,174,95,174);
    drawWhiteLine(94,173,94,199);
    drawWhiteLine(96,173,96,199);
    drawWhiteLine(95,198,102,201);
    drawWhiteLine(95,200,102,203);
    drawWhiteLine(101,202,102,231);
    drawWhiteLine(103,202,104,231);

    drawWhiteLine(106,83,107,28);
    drawWhiteLine(107,28,266,33);
    drawWhiteLine(82,173,82,83);
    drawWhiteLine(82,83,266,83);
    drawWhiteLine(82,118,266,118);
    drawWhiteLine(82,160,266,160);
    drawWhiteLine(25,173,28,46);
    drawWhiteLine(28,46,51,28);
    drawWhiteLine(51,28,80,28);
    drawWhiteLine(80,28,80,16);
    drawWhiteLine(80,16,48,16);
    drawWhiteLine(48,16,21,50);
    drawWhiteLine(21,50,8,280);
}
    
void DrawJalan(int x, int y) {
    // Kelompok 1
    // Garis kiri jalan tamansari (atas-bawah)i : 
    drawWhiteLine(25, 355, 38, 397);
    drawWhiteLine(38, 397, 37, 414);

    // Garis kanan jalan tamansari-garis atas jalan ganesha kiri : 
    drawWhiteLine(30, 354, 41, 393); 
    drawWhiteLine(41, 393, 53, 398); 
    drawWhiteLine(53, 398, 135,374); 
    drawWhiteLine(135, 374, 171, 367); 
    drawWhiteLine(171, 367, 172, 342); 

    // Garis jalan ganesa bawah kiri : 
    drawWhiteLine(50,  414, 58, 408);
    drawWhiteLine(58,  408, 122, 386);
    drawWhiteLine(122, 386, 135, 384);
    drawWhiteLine(135, 384, 147, 395);
    drawWhiteLine(147, 395, 146, 414);

    // Pertigaan skanda (clockwise) : 
    drawWhiteLine(145,384, 154,384); 
    drawWhiteLine(154,384, 151,391); 
    drawWhiteLine(151,391, 145,384);
    
    // Garis jalan ganesa bawah kanan : 
    drawWhiteLine(151, 414, 155, 393); 
    drawWhiteLine(155, 393, 165, 382); 
    drawWhiteLine(165, 382, 200, 382); 
    drawWhiteLine(200, 382, 247, 390); 
    drawWhiteLine(247, 390, 285, 407); 
    drawWhiteLine(285, 407, 285, 414); 

    // Garis jalan kanan bawah :
    drawWhiteLine(289, 414, 289,409);
    drawWhiteLine(289, 409, 292,405);
    drawWhiteLine(292, 405, 356,405);

    // Garis jalan kanan atas : 
    drawWhiteLine(293,337, 291,395);
    drawWhiteLine(291,395, 356,397);

    // Garis atas ganesha kanan : 
    drawWhiteLine(189,255, 189,365);
    drawWhiteLine(189,365, 195,372);
    drawWhiteLine(195,372, 240,380);
    drawWhiteLine(240,380, 285,395);
    drawWhiteLine(285,395, 289,337);

    // Kelompok 3

    drawWhiteLine(246, 334, 288, 334); 
    drawWhiteLine(288, 334, 288, 297); 
    drawWhiteLine(288, 297, 307, 297); 

    drawWhiteLine(246, 337, 288, 337); 
    drawWhiteLine(288, 337, 288, 350);
    drawWhiteLine(292, 337, 292, 301);
    drawWhiteLine(292, 301, 307, 300);

    // Kelompok 4 (307, 234), (315, 234), (311, 239), (311, 297), (307, 300), (307, 233)
    drawWhiteLine(307, 234, 315, 234);
    drawWhiteLine(315, 234, 311, 239);
    drawWhiteLine(311, 239, 311, 297);
    drawWhiteLine(311, 297, 307, 300);
    drawWhiteLine(307, 300, 307, 300);
    drawWhiteLine(307, 300, 307, 234);

    drawWhiteLine(268,196, 302,196);    
    drawWhiteLine(302,196, 302,199);
    drawWhiteLine(302,199, 268,199);

    drawWhiteLine(297,199, 302,199);
    drawWhiteLine(302,199, 303,225);
    drawWhiteLine(303,225, 297,225);




    // Kelompok 6
    drawWhiteLine(128,233, 173,233); 
    drawWhiteLine(173,233, 176,235); 
    drawWhiteLine(176,235, 176,308);

    drawWhiteLine(176,290, 147,290); 
    drawWhiteLine(147,290, 176,288); 
    drawWhiteLine(176,288, 147,288);
    drawWhiteLine(127,240, 125,240);
    drawWhiteLine(125,240, 125,218);
    drawWhiteLine(125,218, 127,218);


}


void drawFrame() {
    drawWhiteLine(0, 0, 1200, 0);
    drawWhiteLine(1200, 0, 1200, 600);
    drawWhiteLine(1200, 600, 0, 600);
    drawWhiteLine(0, 600, 0, 0);
}


void clipper(int y, int x, int size) {
    int i, j;

    drawYellowLine(x-1,y-1,x+size,y-1);
    drawYellowLine(x+size,y-1,x+size,y+size);
    drawYellowLine(x+size,y+size,x-1,y+size);
    drawYellowLine(x-1,y+size,x-1,y-1);

    clearClipperMatrix();

    for (i=0;i<lookSize;i++) {
        for (j=0;j<lookSize;j++) {
            redClipperMatrix[i][j] = redPixelMatrix[x+(i*size/lookSize)][y+(j*size/lookSize)];
            greenClipperMatrix[i][j] = greenPixelMatrix[x+(i*size/lookSize)][y+(j*size/lookSize)];
            blueClipperMatrix[i][j] = bluePixelMatrix[x+(i*size/lookSize)][y+(j*size/lookSize)];
        }
    }
}


void drawClips(int y, int x, int size) {
    int i,j;

    drawYellowLine(x-1,y-1,x+size,y-1);
    drawYellowLine(x+size,y-1,x+size,y+size);
    drawYellowLine(x+size,y+size,x-1,y+size);
    drawYellowLine(x-1,y+size,x-1,y-1);

    for (i=0;i<size;i++) {
        for (j=0;j<size;j++) {
            redPixelMatrix[x+i][y+j] = redClipperMatrix[i][j];
            greenPixelMatrix[x+i][y+j] = greenClipperMatrix[i][j];
            bluePixelMatrix[x+i][y+j] = blueClipperMatrix[i][j];
        }
    }

}

//
//  pohon.cpp
//  
//
//  Created by Raudi on 2/28/17.
//
//

void drawGreenPoint(int x1,int y1){
    if (x1 < 0 || x1 >= WIDTH || y1 < 0 || y1 >= HEIGHT) return;
    redPixelMatrix[x1][y1] = 0;
    greenPixelMatrix[x1][y1] = 255;
    bluePixelMatrix[x1][y1] = 0;
}


void drawGreenLine(int x1, int y1, int x2, int y2) {
    //Than kode lu gua benerin dikit di sini, harusnya ngk usah pake absolut
    int deltaX = x2 - x1;
    int deltaY = y2 - y1;
    int ix = deltaX > 0 ? 1 : -1;
    int iy = deltaY > 0 ? 1 : -1;
    deltaX = abs(deltaX);
    deltaY = abs(deltaY);
    
    int x = x1;
    int y = y1;
    
    drawGreenPoint(x,y);
    
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
            
            drawGreenPoint(x, y);
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
            
            drawGreenPoint(x, y);
        }
    }
}


/*
 
 misal size 5 kurang lebih gini
 | | |*| | |
 | |*| |*| |
 | |*| |*| |
 |*| | | |*|
 |*|*|*|*|*|

*/
void drawPohon(int x, int y, int size) {
    drawGreenLine(x+size-1-(size-1)/2, y, x, y+size-1);
    drawGreenLine(x+(size-1)/2, y, x+size-1, y+size-1);
    drawGreenLine(x, y+size-1, x+size-1, y+size-1);
    floodFill(x+(size-1)/2, y+(size-1)/2, 0,255,0,0,255,0);
    
}

void drawAllPohon() {
    drawPohon(21,14,10);
    drawPohon(40,40,7);
    drawPohon(40,54,7);
    drawPohon(32,65,6);
    drawPohon(32,54,7);
    drawPohon(79,73,7);
    drawPohon(88,62,11);
    drawPohon(34,88,8);
    drawPohon(34,102,8);
    drawPohon(63,141,10);
    drawPohon(128,89,10);
    drawPohon(128,102,10);
    drawPohon(128,125,10);
    drawPohon(128,143,10);
    drawPohon(38,179,13);
    drawPohon(32,210,17);
    drawPohon(194,16,10);
    drawPohon(216,16,10);
    drawPohon(233,16,10);
    drawPohon(250,16,10);
    drawPohon(317,16,29);
    drawPohon(337,132,16);
    drawPohon(251,123,10);
    drawPohon(251,143,10);
    drawPohon(251,88,10);
    drawPohon(251,102,10);
    drawPohon(333,176,19);
    drawPohon(328,217,24);
    drawPohon(316,262,32);
    drawPohon(309,309,36);
    drawPohon(276,87,12);
}

void cutDestroyed () {
    for (int i = 0; i < WIDTH; ++i)
    {
        for (int j = 0; j < HEIGHT; ++j)
        {
            if (destroyed[i][j]) {
                redPixelMatrix[i][j] = 0;
                greenPixelMatrix[i][j] = 0;
                bluePixelMatrix[i][j] = 0;
            }
        }
    }
}

void killMap() {
    for (int i = y_god; i < y_god+size_god; ++i)
    {
        for (int j = x_god; j < x_god+size_god; ++j)
        {
            destroyed[i][j] = true;
        }
    }
}

void drawKeyClip(){
    
    while (!detectKeyStroke()) {
        char KeyPressed = getchar();
        if ((KeyPressed=='D') ||(KeyPressed=='d')) {
            y_god++;
        } else if ((KeyPressed=='A') ||(KeyPressed=='a')) {
            y_god--;
        } else if ((KeyPressed=='S') ||(KeyPressed=='s')) {
            x_god++;
        } else if ((KeyPressed=='W') ||(KeyPressed=='w')) {
            x_god--;
        } else if ((KeyPressed=='I') ||(KeyPressed=='i')) {
            size_god--;
        } else if ((KeyPressed=='O') ||(KeyPressed=='o')) {
            size_god++;
        } else if ((KeyPressed=='P') ||(KeyPressed=='p')) {
            pohon = !pohon;
        } else if ((KeyPressed=='J') ||(KeyPressed=='j')) {
            jalan = !jalan;
        } else if ((KeyPressed=='K') ||(KeyPressed=='k')) {
            killMap();
        } 
    }
}




int main(int argc, char const *argv[]) {

    clearMatrix();

    int fbfd = 0;
    long int screensize = 0;
    bool exploded = false;

    x_god = 50;
    y_god = 50;
    size_god = 100;

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
    screen_size = screensize;

    // Map the device to memory
    fbp = (char *)mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, (off_t)0);

    //display merge center
    // Menulis ke layar tengah file
    //Gambar trapesium
    thread thread1(drawKeyClip);

    int xp = 600;
    int yp = 574;
    char KeyPressed;

    int xawal = 100, yawal = 150;
    bool left = true;

    initDestroyed();

    do {
        clearMatrix();
        drawFrame();
/*
        drawShooter(xp,yp,lastCorrectState);
        drawShooter(xp,yp-150,lastCorrectState);
        drawShooter(xp-200,yp,lastCorrectState);
        drawShooter(xp-200,yp-150,lastCorrectState);
*/
        // draw Pohon
        if (pohon)
            drawAllPohon();

        //draw jalan
        if (jalan)
            DrawJalan2(0,0);

        cutDestroyed();        

        clipper(x_god,y_god,size_god);
        drawClips(50,750,lookSize);

        DrawToScreen();
    } while (KeyPressed!='C');

    munmap(fbp, screensize);
    close(fbfd);

    return 0;
}
