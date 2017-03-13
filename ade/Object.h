#ifndef OBJECT_H
#define OBJECT_H

#include "Line.h"
#include "Point.h"

class Object {
    public :
        Point position;
        Line *lines;
        int nLines;

        Object();
        Object(Point pos);
        void build(Line *line, int nLine);
        void buildRect(int width, int height);
        void buildPolygon(Point *point, int nPoint);

        Object translate (int x, int y);
        Object zoom (double ratioX, double ratioY, Point center);
};
#endif
