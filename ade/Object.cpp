#include "Object.h"
#include "Line.h"
#include "stdio.h"
#include "stdlib.h"
#include <math.h>

using namespace std;

Object :: Object() {
    position = Point(0,0);
    nLines = 0;
}

Object :: Object(Point pos) {
    position = pos;
    nLines = 0;
}

void Object::build(Line *line, int nLine) {
    nLines = nLine;
    lines = (Line*) malloc(nLine * sizeof(Line));
    for (int i = 0; i < nLine; i++) {
        lines[i] = line[i];
    }
}

void Object::buildRect(int width, int height) {
    nLines = 4;
    lines = (Line *) malloc(nLines * sizeof(Line));
    lines[0] = Line (Point(0,0), Point(width, 0));
    lines[1] = Line (Point(width,0), Point(width, height));
    lines[2] = Line (Point(width,height), Point(0, height));
    lines[3] = Line (Point(0,height), Point(0, 0));
}

void Object::buildPolygon(Point* point, int nPoint) {
    nLines =nPoint;
    lines = (Line*) malloc(nLines * sizeof(Line));
    for (int i = 0; i < nLines; i++) {
        lines[i] = Line(point[i%nPoint], point[(i+1)%nPoint]);
    }
}

Object Object::translate(int x, int y) {
    Object ret = Object(Point(position.translate(x,y)));
    ret.build(lines, nLines);
    return ret;
}

Object Object::zoom(double ratioX, double ratioY, Point center) {
    Object ret = Object(position.zoom(ratioX, ratioY, center));
    ret.build(lines, nLines);
    for (int i = 0; i < ret.nLines; i++) {
        ret.lines[i] = lines[i].zoom(ratioX, ratioY, Point(0,0));
    }
    return ret;
}
