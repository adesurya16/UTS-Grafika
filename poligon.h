#ifndef POLIGON_H
#define POLIGON_H

#include <iostream>
#include "line.h"

using namespace std;

class Poligon : public Shape
{
    public:
        Poligon() : arr_Line(), thickness(1), floodfill(Color::BLUE), LineColor(Color::WHITE), xmin(0), xmax(0), ymin(0), ymax(0){}

        Poligon(int Linethickness, Color floodfill, Color LineColor): arr_Line(){
            thickness = Linethickness;
            (*this).floodfill = floodfill;
            (*this).LineColor = LineColor;
        }

        Poligon& operator=(const Poligon& pol){
            arr_Line.clear();
            thickness = pol.thickness;
            floodfill = pol.floodfill;
            LineColor = pol.LineColor;
            for (int i = 0; i < pol.arr_Line.size(); ++i)
            {
                arr_Line.push_back(pol.arr_Line[i]);
            }

            return (*this);
        }

        void add(Line a){
            a.setColor(LineColor);
            a.setThickness(thickness);
            arr_Line.push_back(a);
        }

        Line* getP(unsigned int i){
            if(i < arr_Line.size()){
                return &arr_Line[i];
            }

            return NULL;
        }

        Line* popLast(){
            if(arr_Line.empty()){
                return NULL;
            }
            Line* temp = new Line(arr_Line[arr_Line.size()-1].getP1(), arr_Line[arr_Line.size()-1].getP2(), LineColor,thickness);
            arr_Line.pop_back();
            return temp;
        }

        void draw(FramePanel* a){
            for(unsigned int i = 0;i<arr_Line.size();i++){
                Line ax;
                ax = arr_Line[i];
                ax.setColor(LineColor);
                ax.setThickness(thickness);
                ax.draw(a);
            }
            floodFill((xmax-xmin)/2+xmin, (ymax-ymin)/2+ymin, a);
        }

       

        Color getLineColor(){
            return LineColor;
        }

        Color getFloodfill(){
            return floodfill;
        }

        int getThickness(){
            return thickness;
        }

        void setLineColor(Color c){
            LineColor = c;
        }

        void setFloodfill(Color c){
            floodfill = c;
        }

        void setThickness(int t){
            thickness = t;
        }

        void scalePolygon(float sx, float sy, const  Point& center = Point()){
            for(int i = 0; i < arr_Line.size(); i++){
                arr_Line[i].scaleLine(sx, sy, center);
            }
        }

        void makeLineFromArrPoint(std::vector<Point>& P){
            for(int i = 0; i<P.size(); i++){
                Line l;
                if(i == P.size()-1){
                    l = Line(P[i], P[0]);
                }else{
                    l = Line(P[i], P[i+1]);
                }
                (*this).add(l);
            }
        }


         void drawTree(Point p) {
            Line a,b,c,d,e;

            a = Line(p, Point(p.getX(),p.getY()-5));
            b = Line(Point(p.getX()-4,p.getY()-5),Point(p.getX()+4,p.getY()-5));
            c = Line(Point(p.getX()-4,p.getY()-5),Point(p.getX()-4,p.getY()-9));
            d = Line(Point(p.getX()-4,p.getY()-9),Point(p.getX()+4,p.getY()-9));
            e = Line(Point(p.getX()+4,p.getY()-5),Point(p.getX()+4,p.getY()-9));
            arr_Line.push_back(a);
            arr_Line.push_back(b);
            arr_Line.push_back(c);
            arr_Line.push_back(d);
            arr_Line.push_back(e);
            (*this).setLineColor(Color::GREEN);
            (*this).setThickness(1);
            
            setAllLineColor();
        }

        void setAllLineColor(){
            for(int i = 0; i<arr_Line.size(); i++){
                arr_Line[i].setColor(LineColor);
            }
        }

        void movePolygon(int dx, int dy){
            for(int i = 0; i<arr_Line.size(); i++){
                arr_Line[i].moveLine(dx, dy);
            }
            xmin +=dx;
            xmax +=dx;
            ymin +=dy;
            ymax +=dy;
        }

        void printPolygon(){
            for (int i = 0; i < arr_Line.size(); ++i)
            {
                arr_Line[i].printLine();
            }
        }

        void drawInside(FramePanel* panelNormal, FramePanel* panelZoom){
            for(int i = 0; i < arr_Line.size(); i++){
                Line line;
                float sx = ((float)((*panelZoom).getXSize())/(float)((*panelNormal).getXSize()));
                float sy = ((float)((*panelZoom).getYSize())/(float)((*panelNormal).getYSize()));
                bool a = arr_Line[i].checkInsideFrame(*panelNormal, &line);
                if(a){
                    //cout << "sebel";line.printLine();
                    line.moveLine(((*panelNormal).getXMin()*(-1)), (-1)*((*panelNormal).getYMin()));
                    line.draw(panelNormal);
                    //cout << "sesud"; line.printLine();
                    line.scaleLine(sx, sy);
                    line.draw(panelZoom); 
                    //cout << "sesud2"; line.printLine();                   
                }
            }
        }

        bool checkBitInsideRectangle(int x, int y){
            if(x >= xmin && x<= xmax){
                if(y >= ymin && y <=ymax){
                    return true;
                }
            }

            return false;
        }

        void drawRectangleFromMinMax(int xmin, int xmax, int ymin, int ymax){
            arr_Line.push_back(Line(Point(xmin, ymin),Point(xmax, ymin)));
            arr_Line.push_back(Line(Point(xmax, ymin),Point(xmax, ymax)));
            arr_Line.push_back(Line(Point(xmax, ymax),Point(xmin, ymax)));
            arr_Line.push_back(Line(Point(xmin, ymax),Point(xmin, ymin)));
            (*this).xmin = xmin;
            (*this).xmax = xmax;
            (*this).ymin = ymin;
            (*this).ymax = ymax;
        }

        void floodFill(int x,int y, FramePanel * frame){
            if((x < (*frame).getXSize() && y < (*frame).getYSize() && x > 0 && y > 0)){
                int pause;
                if(!(((*frame).checkColor(LineColor.getR(), LineColor.getG(), LineColor.getB(), x, y)) || 
                    ((*frame).checkColor(floodfill.getR(), floodfill.getG(), floodfill.getB(), x, y)))){
                    if(x < (*frame).getXSize() && y < (*frame).getYSize() && x > 0 && y > 0)    
                        (*frame).set(floodfill.getR(), floodfill.getG(), floodfill.getB(), x, y);
                    floodFill(x,y+1, frame);
                    floodFill(x+1,y, frame);
                    floodFill(x,y-1, frame);
                    floodFill(x-1,y, frame);
                }

            }
        }

    private:
        std::vector<Line> arr_Line;
        int thickness;
        Color floodfill;
        Color LineColor;
        int xmin; //for rectangle
        int xmax; //for rectangle
        int ymin; //for rectangle
        int ymax; //for rectangle
};

#endif
