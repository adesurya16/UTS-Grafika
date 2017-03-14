#ifndef POLIGON_H
#define POLIGON_H

#include <iostream>
#include "line.h"

using namespace std;

class Poligon : public Shape
{
    public:
        Poligon() : arr_Line(), thickness(1), floodfill(Color::BLUE), LineColor(Color::WHITE), minmax(4){}

        Poligon(int Linethickness, Color floodfill, Color LineColor): arr_Line(){
            thickness = Linethickness;
            (*this).floodfill = floodfill;
            (*this).LineColor = LineColor;
        }

        Poligon(const Poligon& pol){
            arr_Line.clear();
            minmax.clear();
            thickness = pol.thickness;
            floodfill = pol.floodfill;
            LineColor = pol.LineColor;
            for (int i = 0; i < pol.arr_Line.size(); ++i)
            {
                arr_Line.push_back(pol.arr_Line[i]);
            }

            for(int i = 0; i<pol.minmax.size(); ++i){
                minmax.push_back(pol.minmax[i]);
            }
        }

        ~Poligon(){
            vector<int>().swap(minmax); //for rectangle
            vector<Line>().swap(arr_Line);
        }

        Poligon& operator=(const Poligon& pol){
            arr_Line.clear();
            minmax.clear();
            thickness = pol.thickness;
            floodfill = pol.floodfill;
            LineColor = pol.LineColor;
            for (int i = 0; i < pol.arr_Line.size(); ++i)
            {
                arr_Line.push_back(pol.arr_Line[i]);
            }

            for(int i = 0; i<pol.minmax.size(); ++i){
                minmax.push_back(pol.minmax[i]);
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
            //cout << "before" << endl;
            for(unsigned int i = 0;i<arr_Line.size();i++){
                Line ax;
                ax = arr_Line[i];
                ax.setColor(LineColor);
                ax.setThickness(thickness);
                ax.draw(a);
            }
            //cout << "draw" << endl;

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
            minmax[0] +=dx;
            minmax[1] +=dx;
            minmax[2] +=dy;
            minmax[3] +=dy;
        }

        void printPolygon(){
            for (int i = 0; i < arr_Line.size(); ++i)
            {
                arr_Line[i].printLine();
            }
            cout << minmax[0] << " , x" << minmax[1] << " ,y" << minmax[2] << " ,y" << minmax[3] << endl;
        }

        void drawInside(FramePanel* panelNormal, FramePanel* panelZoom){
            for(int i = 0; i < arr_Line.size(); i++){
                Line line;
                float sx = ((float)((*panelZoom).getXSize())/(float)((*panelNormal).getXSize()));
                float sy = ((float)((*panelZoom).getYSize())/(float)((*panelNormal).getYSize()));
                bool a = arr_Line[i].checkInsideFrame(*panelNormal, &line);
                if(a){
                    line.moveLine(((*panelNormal).getXMin()*(-1)), (-1)*((*panelNormal).getYMin()));
                    line.draw(panelNormal);
                    line.scaleLine(sx, sy);
                    line.draw(panelZoom);
                }
            }
        }

        bool checkBitInsideRectangle(int x, int y){
            int xmin = minmax[0];
            int xmax = minmax[1];
            int ymin = minmax[2];
            int ymax = minmax[3];
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
            minmax[0]=xmin;
            minmax[1] = xmax;
            minmax[2] = ymin;
            minmax[3] = ymax;
        }

        void rotate(int xmin, int xmax, int ymin, int ymax,int sudut){
            int xpusat = xmin + (xmax-xmin)/2;
            int ypusat = ymin + (ymax-ymin)/2;
            int x1,y1;
            int x2,y2;
            int x3,y3;
            int x4,y4;
            x1 = xpusat + round((xmin-xpusat) * cos(sudut) - (ymin-ypusat) * sin(sudut));
            y1 = ypusat + round((xmin-xpusat) * sin(sudut) + (ymin-ypusat) * cos(sudut));

            x2 = xpusat + round((xmax-xpusat) * cos(sudut) - (ymin-ypusat) * sin(sudut));
            y2 = ypusat + round((xmax-xpusat) * sin(sudut) + (ymin-ypusat) * cos(sudut));

            x3 = xpusat + round((xmax-xpusat) * cos(sudut) - (ymax-ypusat) * sin(sudut));
            y3 = ypusat + round((xmax-xpusat) * sin(sudut) + (ymax-ypusat) * cos(sudut));

            x4 = xpusat + round((xmin-xpusat) * cos(sudut) - (ymax-ypusat) * sin(sudut));
            y4 = ypusat + round((xmin-xpusat) * sin(sudut) + (ymax-ypusat) * cos(sudut));

            arr_Line.push_back(Line(Point(x1, y1),Point(x2, y2)));
            arr_Line.push_back(Line(Point(x2, y2),Point(x3, y3)));
            arr_Line.push_back(Line(Point(x3, y3),Point(x4, y4)));
            arr_Line.push_back(Line(Point(x4, y4),Point(x1, y1)));
        }


        void floodFill(int x,int y, Framebuffer * frame){
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

        int getxmin(){
            return minmax[0];
        }

        int getymin(){
            return minmax[2];
        }

        int getxmax(){
            return minmax[1];
        }

        int getymax(){
            return minmax[3];
        }

    private:
        vector<int> minmax; //for rectangle
        std::vector<Line> arr_Line;
        int thickness;
        Color floodfill;
        Color LineColor;

};

#endif
