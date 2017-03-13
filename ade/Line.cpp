#include "Line.h"
#include "Point.h"
#include <stdio.h>

using namespace std;

Line :: Line() {
    p1 = Point(0,0);
    p2 = Point(0,0);
}

Line :: Line(Point np1, Point np2) {
    p1 = np1;
    p2 = np2;
}

Line Line::translate(int x, int y) {
    return Line(p1.translate(x,y), p2.translate(x,y));
}

Line Line::zoom(double ratioX, double ratioY, Point center) {
    return Line(p1.zoom(ratioX, ratioY, center), p2.zoom(ratioX, ratioY, center));
}

