#include "Point.h"
#include <stdio.h>
#include <math.h>

using namespace std;

Point :: Point() {
    x = 0;
    y = 0;
}
Point :: Point(int nx, int ny) {
    x = nx;
    y = ny;
}

Point Point::translate(int dx, int dy) {

    return Point(x + dx, y + dy);
}

Point Point::zoom(double ratioX, double ratioY, Point center) {
    int newX = (int) round(center.x + ratioX*(x - center.x));
    int newY = (int) round(center.y + ratioY*(y - center.y));
    return Point(newX, newY);
};


