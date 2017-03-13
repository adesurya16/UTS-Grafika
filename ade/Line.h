#ifndef LINE_H
#define LINE_H

#include "Point.h"

using namespace std;

class Line {
    public :
        Point p1;
        Point p2;
        Line();
        Line(Point np1, Point np2);
        Line translate (int x, int y);
        Line zoom (double ratioX, double ratioY, Point center);
};
#endif
