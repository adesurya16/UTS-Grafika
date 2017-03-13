#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <cstdio>
#include <termios.h>
#include <math.h>
#include <vector>

using namespace std;

#define HEIGHT 80
#define WIDTH 60

int redPixelMatrix[WIDTH][HEIGHT];
int greenPixelMatrix[WIDTH][HEIGHT];
int bluePixelMatrix[WIDTH][HEIGHT];

struct bullet
{
    int xStart;
    int yStart;
    int xEnd;
    int yEnd;
    float m;
    float c;
    int partisi;
    int iteration;
    int x1;
    int x2;
    int n;
};

vector<bullet> bullets;

void drawWhitePoint(int x1, int y1) {
    redPixelMatrix[x1][y1] = 255;
    greenPixelMatrix[x1][y1] = 255;
    bluePixelMatrix[x1][y1] = 255;
}

void drawWhiteLine(int x1, int y1, int x2, int y2) {
	//Than kode lu gua benerin dikit di sini, harusnya ngk usah pake absolut
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
 
            drawWhitePoint(x, y);
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

void printMatrix() { // print ke stdio
    for (int i = 0; i < WIDTH; ++i)
    {
        for (int j = 0; j < HEIGHT; ++j)
        {
            if (bluePixelMatrix[i][j] != 0) {
                cout << 0;
            } else {
                cout << '-';
            }
        }
        cout << endl;
    }
}

void addBullet(int x1, int y1, int x2, int y2 , int n)
//x1,y1 titik asal peluru
//x2,y2 titik sampai peluru
//n adalah pembagian tahap gerak peluru
{
	bullet newBullet;
    //persamaan garis
    newBullet.m = (y2-y1);
    newBullet.m /= (x2-x1);
    newBullet.c = y1 - newBullet.m * x1;

    newBullet.partisi = 0;
    for (int i=1;i<=n;i++) {
        newBullet.partisi += i;
    }


    newBullet.xStart = x1;
    newBullet.yStart = (int) floor(newBullet.m * newBullet.xStart + newBullet.c + 0.5);
    newBullet.xEnd = x1 + (x2-x1) * n / newBullet.partisi;
    newBullet.yEnd = (int) floor(newBullet.m * newBullet.xEnd + newBullet.c + 0.5);

    newBullet.x1 = x1;
    newBullet.x2 = x2;
    newBullet.iteration = 0;
    newBullet.n = n;

    bullets.push_back(newBullet);
}

void drawBullets() {
    //persamaan garis
    for (int i = 0; i < bullets.size(); ++i)
    {
    	if (bullets[i].iteration <= bullets[i].n) {
    		if (drawWhiteLine(bullets[i].xStart,bullets[i].yStart,bullets[i].xEnd,bullets[i].yEnd)) exploded = true;
			bullets[i].xStart = bullets[i].xEnd;
			bullets[i].yStart = bullets[i].yEnd;
			bullets[i].xEnd = bullets[i].xStart + (bullets[i].x2 - bullets[i].x1) * (bullets[i].iteration - 1) / bullets[i].partisi;
			bullets[i].yEnd = (int) floor(bullets[i].m * bullets[i].xEnd + bullets[i].c + 0.5);
			bullets[i].iteration++;
    	}
    }
}

void drawBullet(int x1, int y1, int x2, int y2 , int n)
//x1,y1 titik asal peluru
//x2,y2 titik sampai peluru
//n adalah pembagian tahap gerak peluru
{
    //persamaan garis
    float m = (y2-y1);
    m /= (x2-x1);
    float c = y1 - m*x1;

    int partisi = 0;
    for (int i=1;i<=n;i++) {
        partisi += i;
    }


    int xStart = x1;
    int yStart = (int) floor(m*xStart+c+0.5);
    int xEnd = x1+(x2-x1)*n/partisi;
    int yEnd = (int) floor(m*xEnd+c+0.5);
    for (int i=n;i>0;i--) {
        drawWhiteLine(xStart,yStart,xEnd,yEnd);
        DrawToScreen();
        usleep(3000000*i/partisi);
        eraseWithBlackBox(xEnd,yEnd,xStart,yStart);
        xStart = xEnd;
        yStart = yEnd;
        xEnd = xStart+(x2-x1)*(i-1)/partisi;
        yEnd = (int) floor(m*xEnd+c+0.5);
    }
}

int main() {
    //printf("masuk\n");
    clearMatrix();

    do {
        clearMatrix();
        usleep(1000000);
    } while (bullets.size() < 10);
    
    return 0;
}