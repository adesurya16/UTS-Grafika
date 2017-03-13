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

using namespace std;

#define HEIGHT 1250
#define WIDTH 650

struct fb_var_screeninfo vinfo;
struct fb_fix_screeninfo finfo;
char *fbp = 0;

int xp, yp;
int redPixelMatrix[WIDTH][HEIGHT];
int greenPixelMatrix[WIDTH][HEIGHT];
int bluePixelMatrix[WIDTH][HEIGHT];
int posX;
int posY;
int lastCorrectState = 's';
bool exploded = false;
bool paused = false;

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

void clearMatrix() {
    for (int i = 0; i < 600; ++i)
    {
        for (int j = 0; j < 1200; ++j)
        {
            redPixelMatrix[i][j] = 0;
            greenPixelMatrix[i][j] = 0;
            bluePixelMatrix[i][j] = 0;
        }
    }
}

void drawWhitePoint(int x1, int y1) {
    redPixelMatrix[x1][y1] = 255;
    greenPixelMatrix[x1][y1] = 255;
    bluePixelMatrix[x1][y1] = 255;
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

void drawSemiCircle(int x0, int y0, int radius)
{
    int x = radius;
    int y = 0;
    int err = 0;

    while (x >= y)
    {
        drawWhitePoint(x0 - x, y0 + y);
        drawWhitePoint(x0 - y, y0 + x);
        drawWhitePoint(x0 - y, y0 - x);
        drawWhitePoint(x0 - x, y0 - y);

        if (err <= 0)
        {
            y += 1;
            err += 2*y + 1;
        }
        if (err > 0)
        {
            x -= 1;
            err -= 2*x + 1;
        }
    }

    //warnain
    floodFill(x0-5,y0,255,255,255,255,255,0);
}

void drawCircle(int x0, int y0, int radius)
{
    int x = radius;
    int y = 0;
    int err = 0;

    while (x >= y)
    {
        drawWhitePoint(x0 - x, y0 + y);
        drawWhitePoint(x0 - y, y0 + x);
        drawWhitePoint(x0 - y, y0 - x);
        drawWhitePoint(x0 - x, y0 - y);
        drawWhitePoint(x0 + x, y0 + y);
        drawWhitePoint(x0 + y, y0 + x);
        drawWhitePoint(x0 + y, y0 - x);
        drawWhitePoint(x0 + x, y0 - y);

        if (err <= 0)
        {
            y += 1;
            err += 2*y + 1;
        }
        if (err > 0)
        {
            x -= 1;
            err -= 2*x + 1;
        }
    }
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

void drawShooter(int xp, int yp, char mode) {
    //gambar tembakan dengan titik pusat lingkaran tembakan
    //(yp,xp)

    posX = xp;
    posY = 500;
    drawCircle(yp,xp,25);
    floodFill(yp, xp, 255, 255, 255, 255, 0, 0);
    drawWhiteLine(yp-15,xp+20,yp-50,xp+20);
    drawWhiteLine(yp-15,xp-20,yp-50,xp-20);
    drawWhiteLine(yp-50,xp+20,yp-50,xp-20);
    floodFill(yp-30, xp+10, 255, 255, 255, 0, 0, 255);

}

void DrawToScreen(){
    /* prosedure yang menggambar ke layar dari matriks RGB (harusnya rata tengah)*/
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

void drawExplosion(int x,int y){
    //x = 70
    // bentuk bintang ada 8 garis sesuai dengan parameter titik pusat (x,y)
    int pointx1 = x-20, pointy1 =y+20;
    int pointx3 = x+20, pointy3 =y+20;
    int pointx5 = x+20, pointy5 =y-20;
    int pointx7 = x-20, pointy7 =y-20;

    int pointx2 = x, pointy2 = y+10;
    int pointx4 = x+10, pointy4 = y;
    int pointx6 = x, pointy6 = y-10;
    int pointx8 = x-10, pointy8 = y;

    //gambar ledakan
    drawRedLine(pointx1,pointy1,pointx2,pointy2);
    drawRedLine(pointx2,pointy2,pointx3,pointy3);
    drawRedLine(pointx3,pointy3,pointx4,pointy4);
    drawRedLine(pointx4,pointy4,pointx5,pointy5);
    drawRedLine(pointx5,pointy5,pointx6,pointy6);
    drawRedLine(pointx6,pointy6,pointx7,pointy7);
    drawRedLine(pointx7,pointy7,pointx8,pointy8);
    drawRedLine(pointx8,pointy8,pointx1,pointy1);

    //warnain
    floodFill(x,y,255,0,0,255,255,0);
}


void drawExplosion2(int x,int y){
    //x = 70
    // bentuk bintang ada 8 garis sesuai dengan parameter titik pusat (x,y)
    int pointx1 = x-20, pointy1 =y+20;
    int pointx3 = x+30, pointy3 =y+30;
    int pointx5 = x+20, pointy5 =y-20;
    int pointx7 = x-30, pointy7 =y-30;

    int pointx2 = x, pointy2 = y+15;
    int pointx4 = x+10, pointy4 = y;
    int pointx6 = x, pointy6 = y-10;
    int pointx8 = x-15, pointy8 = y;

    //gambar ledakan
    drawRedLine(pointx1,pointy1,pointx2,pointy2);
    drawRedLine(pointx2,pointy2,pointx3,pointy3);
    drawRedLine(pointx3,pointy3,pointx4,pointy4);
    drawRedLine(pointx4,pointy4,pointx5,pointy5);
    drawRedLine(pointx5,pointy5,pointx6,pointy6);
    drawRedLine(pointx6,pointy6,pointx7,pointy7);
    drawRedLine(pointx7,pointy7,pointx8,pointy8);
    drawRedLine(pointx8,pointy8,pointx1,pointy1);

    //warnain
    floodFill(x,y,255,0,0,255,0,0);
}

void drawUFO(int x1, int y1) {
    drawWhiteLine(x1, y1, x1+20, y1+20);
    drawWhiteLine(x1, y1, x1, y1-50);
    drawWhiteLine(x1, y1-50, x1+20, y1-70);
    drawWhiteLine(x1+20, y1-70, x1+20, y1+20);

    floodFill(x1+5, y1, 255, 255, 255, 0, 255, 0);

    drawSemiCircle(x1, y1-25, 25);
}

void drawStars() {
    drawExplosion(400, 100);
    drawExplosion(300, 200);
    drawExplosion(500, 200);
    drawExplosion(400, 300);
    drawExplosion(300, 400);
    drawExplosion(500, 400);
    drawExplosion(400, 500);
    drawExplosion(400, 700);
    drawExplosion(300, 800);
    drawExplosion(500, 800);
    drawExplosion(400, 900);
    drawExplosion(300, 1000);
    drawExplosion(500, 1000);
    drawExplosion(400, 1100);
}

void drawFrame() {
    drawWhiteLine(0, 0, 0, 1200);
    drawWhiteLine(0, 1200, 600, 1200);
    drawWhiteLine(600, 1200, 600, 0);
    drawWhiteLine(600, 0, 0, 0);
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
    newBullet.iteration = n;
    newBullet.n = n;

    bullets.push_back(newBullet);
}

void drawKeyShooter() {
    while(!exploded) {
        if(!detectKeyStroke()) {
            char KeyPressed = getchar();

            if ((KeyPressed=='A')||(KeyPressed=='a'))
                xp--;
            else if ((KeyPressed=='D')||(KeyPressed=='d'))
                xp++;
            else if (KeyPressed==' ')
                addBullet(posY,posX,0,xp,20);
            else if ((KeyPressed=='P') || (KeyPressed=='p')) {
                paused = true;
                while (paused) {
                    if (!detectKeyStroke()) {
                        char KeyPressed = getchar();
                        if ((KeyPressed=='P') || (KeyPressed=='p')) {
                            paused = false;
                        }
                    }
                }
            }
        }
    }
}

bool getPaused() {
    return paused;
}

void drawBullets() {
    //persamaan garis
    for (int i = bullets.size()-1; i >=0; --i)
    {
        if (bullets[i].iteration >0) {
            if (drawWhiteLine(bullets[i].xStart,bullets[i].yStart,bullets[i].xEnd,bullets[i].yEnd)) exploded = true;
            bullets[i].xStart = bullets[i].xEnd;
            bullets[i].yStart = bullets[i].yEnd;
            bullets[i].xEnd = bullets[i].xStart + (bullets[i].x2 - bullets[i].x1) * (bullets[i].iteration - 1) / bullets[i].partisi;
            bullets[i].yEnd = (int) floor(bullets[i].m * bullets[i].xEnd + bullets[i].c + 0.5);
            bullets[i].iteration--;
        }
    }
}

void moveBounce(double* x, double* y, double* vx, double* vy, int yground) {

  double a = 10;

  double x2 = *x + *vx;
  double vx2 = *vx;

  if(x2 <= 0) {
    x2 *= -1;
    vx2 *= -1;
  }

  if(x2 >= 1200) {
    x2 = 2400 - x2;
    vx2 *= -1;
  }

  double y2 = *y + *vy + a/2;
  double vy2 = *vy + a;

  if(y2 > yground) {
    double t0 = (-(*vy) + sqrt((*vy)*(*vy)-2*a*(*y - yground)))/a;
    double v0 = -(*vy + a*t0);

    if(a*(v0/a)*(v0/a)/2 + v0*(-v0/a) + yground > yground-1) {
      y2 = yground;
      vy2 = 0;
    } else {
      y2 = (a*(1-t0)*(1-t0)/2 + v0*(1-t0) + yground);
      vy2 = (v0 + a*(1-t0)) * 0.8;
    }
  }

  *x = x2;
  *y = y2;
  *vx= vx2;
  *vy = vy2;
}

void drawwheel(int x1,int y1,int r,double sudut){
// r > 20
//(0,)

  int x2 = r-20;
  int y2 = 0;

  int x3 = 0;
  int y3 = r-20;

  int x22 = round(x2 * cos(sudut) - y2 * sin(sudut));
  int y22 = round(x2 * sin(sudut) + y2 * cos(sudut));

  int x32 = round(x3 * cos(sudut) - y3 * sin(sudut));
  int y32 = round(x3 * sin(sudut) + y3 * cos(sudut));



  drawCircle(x1,y1,r-20);
  drawWhiteLine(x1+x22,y1+y22,x1-x22,y1-y22);
  drawWhiteLine(x1+x32,y1+y32,x1-x32,y1-y32);

//gambar 2 garis didalam roda


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

    // Map the device to memory
    fbp = (char *)mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, (off_t)0);

    //display merge center
    // Menulis ke layar tengah file
    //Gambar trapesium
    thread thread1(&drawKeyShooter);
    xp = 600;
    yp = 574;
    char KeyPressed;

    int xawal = 100, yawal = 1180;
    bool left = true;

    do {

        while (getPaused()) {
            if (!getPaused())
                break;
        }

        clearMatrix();
        drawFrame();

        drawShooter(xp,yp,lastCorrectState);
        drawStars();

        // draw UFO
        drawUFO(xawal, yawal);
        if(yawal-70<=0) {
            left = false;
        } else if(yawal+20>=1200) {
            left = true;
        }
        if (left) {
            yawal -= 10;
        } else {
            yawal += 10;
        }

        // draw bullet
        drawBullets();

        DrawToScreen();
        usleep(50000);


    } while (!exploded);

    thread1.detach();
    clearMatrix();
    drawFrame();
    drawShooter(xp,yp,lastCorrectState);
    drawStars();
    drawExplosion2(xawal,yawal);
    DrawToScreen();


    munmap(fbp, screensize);
    close(fbfd);

    return 0;
}
