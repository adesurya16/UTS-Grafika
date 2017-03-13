#ifndef POINT_H
#define POINT_H

using namespace std;

class Point {
    public :
        int x;
        int y;
        Point();
        Point(int nx, int ny);
        Point translate(int dx, int dy);
        Point zoom(double ratioX, double ratioY, Point center);
};
#endif // POINT_H
