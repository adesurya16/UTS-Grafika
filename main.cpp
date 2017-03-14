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
#include "poligon.h"
#include "parser.h"

using namespace std;

#define HEIGHT 1250
#define WIDTH 650

struct fb_var_screeninfo vinfo;
struct fb_fix_screeninfo finfo;
char *fbp = 0;

int width;
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

std::vector<Poligon> vPoligon;

vector<bullet> bullets;
vector<vector<Poligon>> vKotak;

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

void drawWhitePoint(int x1, int y1, FramePanel * fb) {
    if(x1 < (*fb).getXSize() && y1 < (*fb).getYSize() && x1 > 0 && y1 > 0)
        (*fb).set(255,255,255,x1, y1);
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

void floodFill(int x,int y,int redBatas,int greenBatas,int blueBatas,int redColor,int greenColor,int blueColor, Framebuffer * frame){
    if((x < (*frame).getXSize() && y < (*frame).getYSize() && x > 0 && y > 0)){
        //(*frame).printColor(x,y);
        //cout << "ini batas "<< greenBatas << " " << redBatas <<" " << blueBatas << endl;
        //(*frame).Draw();
        int pause;
        //usleep(1000);
        if(!(((*frame).checkColor(redBatas, greenBatas, blueBatas, x, y)) ||
            ((*frame).checkColor(redColor, greenColor, blueColor, x, y)))){
            if(x < (*frame).getXSize() && y < (*frame).getYSize() && x > 0 && y > 0)
                (*frame).set(redColor, greenColor, blueColor, x, y);
            floodFill(x,y+1,redBatas,greenBatas,blueBatas,redColor,greenColor,blueColor, frame);
            floodFill(x+1,y,redBatas,greenBatas,blueBatas,redColor,greenColor,blueColor, frame);
            floodFill(x,y-1,redBatas,greenBatas,blueBatas,redColor,greenColor,blueColor, frame);
            floodFill(x-1,y,redBatas,greenBatas,blueBatas,redColor,greenColor,blueColor, frame);
        }

    }
}

void drawCircle(int x0, int y0, int radius, FramePanel * frame)
{
    int x = radius;
    int y = 0;
    int err = 0;

    while (x >= y)
    {
        drawWhitePoint(x0 - x, y0 + y, frame);
        drawWhitePoint(x0 - y, y0 + x, frame);
        drawWhitePoint(x0 - y, y0 - x, frame);
        drawWhitePoint(x0 - x, y0 - y, frame);
        drawWhitePoint(x0 + x, y0 + y, frame);
        drawWhitePoint(x0 + y, y0 + x, frame);
        drawWhitePoint(x0 + y, y0 - x, frame);
        drawWhitePoint(x0 + x, y0 - y, frame);

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

bool drawWhiteLine(int x1, int y1, int x2, int y2, FramePanel * frame) {
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

    drawWhitePoint(x,y,frame);

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
            drawWhitePoint(x, y, frame);
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
            drawWhitePoint(x, y, frame);
        }
    }
    return ret;
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

void drawShooter(int xp, int yp, char mode, Framebuffer * frame) {
    //gambar tembakan dengan titik pusat lingkaran tembakan
    //(yp,xp)

    posX = xp;
    posY = 500;
    drawCircle(xp,yp,25, frame);
    floodFill(xp, yp, 255, 255, 255, 255, 0, 0, frame);
    drawWhiteLine(xp+20,yp-15,xp+20,yp-50, frame);
    drawWhiteLine(xp-20,yp-15,xp-20,yp-50, frame);
    drawWhiteLine(xp+20,yp-50,xp-20,yp-50, frame);
    floodFill(xp+10, yp-30, 255, 255, 255, 0, 0, 255, frame);

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

void addBullet(int x1, int y1, int x2, int y2 , int n)
//x1,y1 titik asal peluru
//x2,y2 titik sampai peluru
//n adalah pembagian tahap gerak peluru
{
    bullet newBullet;
    //persamaan garis
    newBullet.m = (y2-y1);
    newBullet.m /= (x2-x1);
    if(x2 - x1 == 0){
        newBullet.m = 0;
    }
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

            if ((KeyPressed=='A')||(KeyPressed=='a')){
                xp -= 10;
            }else if ((KeyPressed=='D')||(KeyPressed=='d')){
                xp += 10;
            } else if ((KeyPressed=='W')||(KeyPressed=='w')){
                width = width+5;
            } else if ((KeyPressed=='Q')||(KeyPressed=='q')){
                width = width-5;
            }
            else if (KeyPressed==' '){
                addBullet(posX,posY,xp+width,0,20);
                addBullet(posX,posY,xp-width,0,20);
                //cout << posX << endl;
            }
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

void drawBullets(FramePanel * fp) {
    //persamaan garis
    std::vector<bullet>::iterator it;
    for (it = bullets.begin(); it !=bullets.end();)
    {
        if ((*it).iteration >0) {
            if (drawWhiteLine((*it).xStart,(*it).yStart,(*it).xEnd,(*it).yEnd, fp)) exploded = true;
            (*it).xStart = (*it).xEnd;
            (*it).yStart = (*it).yEnd;
            (*it).xEnd = (*it).xStart + ((*it).x2 - (*it).x1) * ((*it).iteration - 1) / (*it).partisi;
            (*it).yEnd = (int) floor((*it).m * (*it).xEnd + (*it).c + 0.5);
            (*it).iteration;
        }
        if(((*it).iteration == 0)){
            it = bullets.erase(it);
        }else{
            it++;
        }
    }
}

void moveBounce(double* y, double* x, double* vy, double* vx, int yground) {

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

void drawwheel(int x1,int y1,int r,double sudut,FramePanel *fp){
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

  drawCircle(x1,y1,r-20,fp);
  drawWhiteLine(x1+x22,y1+y22,x1-x22,y1-y22,fp);
  drawWhiteLine(x1+x32,y1+y32,x1-x32,y1-y32,fp);
//gambar 2 garis didalam roda
}

void fall(int x,int y){
  int jatuh = 0;
  double xw1 = x;
  double yw1 = y;
  double vxw1 = 0;
  double vyw1 = 10;
  int angle1 = 0;
  vector<Poligon> vPergerakan;
  while(jatuh<100){
    moveBounce(&yw1, &xw1, &vyw1, &vxw1, 800);
    // yw1++;
    // xw1++;
    // vyw1++;
    // vxw1++;

    Poligon pl;
    int xmin = round(xw1) - 10;
    int xmax = round(xw1) + 10;
    int ymin = round(yw1) - 10;
    int ymax = round(yw1) + 10;
    pl.rotate(xmin,xmax,ymin,ymax,angle1);
    //cout << xmin << "AAAAAAAAAAAAAAAAAAAAAAAAAA" << endl;
    //drawwheel(round(xw1), round(yw1), 30, angle1*M_PI/10,fp);
    angle1 = (angle1+1)%10;
    jatuh++;

    vPergerakan.push_back(pl);
  }
  vKotak.push_back(vPergerakan);
}

void pecahanjatuh(){
    while(true){
      /*
        for(int i=0;i<pecahan.size();i++){
          pecahan.push_back();
        }
        */
    }
}


void spawnPolygon(vector<vector<Point>> mPoint) {
  for(int i = 0; i < mPoint.size(); i++){
      int xmin, xmax, ymin, ymax;
      xmin = mPoint[i][0].getX();
      xmax = mPoint[i][0].getY();
      ymin = mPoint[i][1].getX();
      ymax = mPoint[i][1].getY();
      Poligon * temp = new Poligon();
      (*temp).drawRectangleFromMinMax(xmin, xmax, ymin, ymax);
      vPoligon.push_back((*temp));
  }
}


int main() {
    // mendapat screensize layar monitor

    // Map the device to memory
    Framebuffer fb;
    fbp = fb.getfbp();

    //display merge center
    // Menulis ke layar tengah file
    //Gambar trapesium
    thread thread1(&drawKeyShooter);

    xp = 600;
    yp = 574;
    width = 30;
    char KeyPressed;

    int xawal = 100, yawal = 1180;
    bool left = true;

    int npola = 5;

    //Read Data
    std::vector<std::vector<std::vector<Point>>> allmPoint;
    Parser parse;
    std::vector<std::vector<Point>> mPoint;
    
    for (int i=0;i<npola;i++) {
        stringstream ss;
        ss << "banguna" << i+1;
        parse.parseAdi(ss.str());  
        mPoint = parse.getPoints();
        allmPoint.push_back(mPoint);
    }
    
    fb.Draw();
    int pause;
    int moveCounter = 1;
    int lebihGedeDariPause = 10;
    int persen;
    int oi;


    for(;;){
        moveCounter++;
        persen =  300/lebihGedeDariPause;
        if(moveCounter%persen == 0) {
            oi = rand() % npola;
            spawnPolygon(allmPoint.at(oi));
        }

        while (getPaused()) {
            if (!getPaused()){

              break;
            }

        }
        fb.EmptyFrame();
        std::vector<Poligon>::iterator it = vPoligon.begin();
        while(it != vPoligon.end()){
            bool checkDraw = true;
            if(pause > 5){
                //cout << "hhh" << endl;
                (*it).movePolygon(0,5);
            }
            std::vector<bullet>::iterator itbullet = bullets.begin();
            while(itbullet != bullets.end() && checkDraw != false){
                if((*it).checkBitInsideRectangle((*itbullet).xEnd, (*itbullet).yEnd)){
                    checkDraw = false;
                    itbullet = bullets.erase(itbullet);
                }else{
                    itbullet++;
                }
            }
            if(checkDraw == true){
                (*it).draw(&fb);
                int xo = ((*it).getxmax()-(*it).getxmin())/2+(*it).getxmin();
                int yo = ((*it).getymax()-(*it).getymin())/2+(*it).getymin();
                (*it).floodFill(xo, yo, &fb);
                ++it;
            }else{
                Poligon tertembak = (*it);
                int xawalpecahan = (tertembak.getxmax()-tertembak.getxmin())/2+tertembak.getxmin();
                int yawalpecahan = (tertembak.getymax()-tertembak.getymin())/2+tertembak.getymin();
                it = vPoligon.erase(it);
                fall(xawalpecahan, yawalpecahan);
                //thread threadpecahan(&fall,xawalpecahan,yawalpecahan,&fb);
                //cout << xawalpecahan << " " << yawalpecahan <<endl;
            }
        }
        if(pause > lebihGedeDariPause ){
            pause  = 0;
        }
        for (int i=0; i<vKotak.size(); i++) {
          if(!vKotak[i].empty()) {
              Poligon pl = vKotak[i][0];
              //cout << i << "AAAAAAAAAAAAAAAAAAAAAAAAAAAAA" << endl;
              //pl.printPolygon();
              pl.draw(&fb);
              //vKotak[i].pop_back();
              vKotak[i].erase(vKotak[i].begin());
          }
          //vKotak[i].
        }
        drawShooter(xp,yp,lastCorrectState, &fb);
        drawBullets(&fb);
        fb.Draw();
        usleep(100);
        pause++;
    }
    //threadpecahan.detach();
    thread1.detach();

    return 0;
}
