#include<bits/stdc++.h>
#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include <windows.h>
#include <GL\glut.h>

#define dbg(x) std::cout<<#x<<": "<<x<<std::endl;

#define pi (2*acos(0.0))

#define AMBIENT 0
#define DIFFUSE 1
#define SPECULAR 2
#define REFLECTION 3

using namespace std;

class Object;
class PointLight;
class SpotLight;

extern vector<Object*> objects;
extern vector<PointLight*> pointLights;
extern vector<SpotLight*> spotLights;
extern int recursion_level;

struct Point{
    double x, y, z;
    Point(double x=0, double y=0, double z=0){
        this->x = x;
        this->y = y;
        this->z = z;
    }

    // for debug
    void Print(){
        dbg(x);
        dbg(y);
        dbg(z);
    }
};

struct Vector{
    double x, y, z;
    Vector(double x=0, double y=0, double z=0){
        this->x = x;
        this->y = y;
        this->z = z;
    }

    void Normalize(){
        double value = sqrt(x*x + y*y + z*z);
        x = x/value;
        y = y/value;
        z = z/value;
    }

    static double DotProduct(Vector &a, Vector &b) {
        return (a.x * b.x + a.y * b.y + a.z * b.z);
    }
    static Vector CrossProduct(Vector &a, Vector &b) {
        Vector cross;
        cross.x = a.y * b.z - a.z * b.y;
        cross.y = a.z * b.x - a.x * b.z;
        cross.z = a.x * b.y - a.y * b.x;
        return cross;
    }

    // for debug
    void Print(){
        dbg(x);
        dbg(y);
        dbg(z);
    }
};


struct Color{
    double r,g,b;
    Color(){
        r=g=b=1.0;
    }
    Color(double R, double G, double B){
        r = R;
        g = G;
        b = B;
    }

    void Normalize(){
        if(r < 0) r = 0.0;
        if(r > 1) r = 1.0;
        if(g < 0) g = 0.0;
        if(g > 1) g = 1.0;
        if(b < 0) b = 0.0;
        if(b > 1) b = 1.0;

    }
};



class Object {
protected:
    Point reference_point;
    double height, width, length;
    Color color;
    double coefficients[4];
    // 0 -> ambient coefficients
    // 1 -> diffuse coefficients
    // 2 -> specular coefficients
    // 3 -> reflection coefficients
    int shine; // exponent term of specular component

public:
    Object(){
        height = 0;
        width = 0;
        length = 0;
        coefficients[AMBIENT] = 0;
        coefficients[DIFFUSE] = 0;
        coefficients[SPECULAR] = 0;
        coefficients[REFLECTION] = 0;
        shine = 0;
    }

    virtual void draw(){
        // this function will be overridden
    }

    void setColor(Color color){
        this->color = color;
        this->color.Normalize();
    }

    void setColor(double R, double G, double B){
        this->color.r = R;
        this->color.g = G;
        this->color.b = B;
        this->color.Normalize();
    }

    void setShine(int s){
        this->shine = s;
    }

    void setCoefficients(double a, double d, double s, double r){
        this->coefficients[AMBIENT] = a;
        this->coefficients[DIFFUSE] = d;
        this->coefficients[SPECULAR] = s;
        this->coefficients[REFLECTION] = r;
    }

};

class Sphere : public Object {
public:

    Sphere(){
        reference_point = Point();
        length = 0;
    }

    Sphere(Point center, double radius){
        reference_point = center;
        length = radius;
    }

    void draw() {
        glPushMatrix();
        glTranslatef(reference_point.x, reference_point.y, reference_point.z);
        struct Point points[100][100];
        int i,j;
        double h,r;
        double radius = this->length;
        int stacks = 50;
        int slices = 50;
        //generate points
        for(i=0;i<=stacks;i++){
            h=radius*sin(((double)i/(double)stacks)*(pi/2));
            r=radius*cos(((double)i/(double)stacks)*(pi/2));
            for(j=0;j<=slices;j++){
                points[i][j].x=r*cos(((double)j/(double)slices)*2*pi);
                points[i][j].y=r*sin(((double)j/(double)slices)*2*pi);
                points[i][j].z=h;
            }
        }
        //draw quads using generated points
        for(i=0;i<stacks;i++){
            glColor3f((double)i/(double)stacks,(double)i/(double)stacks,(double)i/(double)stacks);
            for(j=0;j<slices;j++){
                glBegin(GL_QUADS);{
                    //upper hemisphere
                    glVertex3f(points[i][j].x,points[i][j].y,points[i][j].z);
                    glVertex3f(points[i][j+1].x,points[i][j+1].y,points[i][j+1].z);
                    glVertex3f(points[i+1][j+1].x,points[i+1][j+1].y,points[i+1][j+1].z);
                    glVertex3f(points[i+1][j].x,points[i+1][j].y,points[i+1][j].z);
                    //lower hemisphere
                    glVertex3f(points[i][j].x,points[i][j].y,-points[i][j].z);
                    glVertex3f(points[i][j+1].x,points[i][j+1].y,-points[i][j+1].z);
                    glVertex3f(points[i+1][j+1].x,points[i+1][j+1].y,-points[i+1][j+1].z);
                    glVertex3f(points[i+1][j].x,points[i+1][j].y,-points[i+1][j].z);
                }glEnd();
            }
        }
        glPopMatrix();
    }

};

class Triangle : public Object {
public:
    Point points[3];

    Triangle(){}

    Triangle(Point a, Point b, Point c){
        this->points[0] = a;
        this->points[1] = b;
        this->points[2] = c;
    }

    void draw(){
        glPushMatrix();
        glColor3f(color.r, color.g, color.b);
        glBegin(GL_TRIANGLES);{
            glVertex3f(points[0].x, points[0].y, points[0].z);
            glVertex3f(points[1].x, points[1].y, points[1].z);
            glVertex3f(points[2].x, points[2].y, points[2].z);
        }glEnd();
        glPopMatrix();
    }
};

class GeneralShape : public Object {
public:
    double A,B,C,D,E,F,G,H,I,J;

    GeneralShape(){
        A=B=C=D=E=F=G=H=I=J=0;
    }

    GeneralShape(double a, double b, double c, double d,
                 double e, double f, double g, double h,
                 double i, double j, double length,
                 double width, double height, Point &pos){
        this->reference_point = pos;
        this->length = length;
        this->width = width;
        this->height = height;
        A = a;
        B = b;
        C = c;
        D = d;
        E = e;
        F = f;
        G = g;
        H = h;
        I = i;
        J = j;
    }

    void draw(){
        glPushMatrix();
        // no need to draw this

        glPopMatrix();
    }

};

class Floor : public Object {
public:
    int tileCount;
    double tileWidth;

    Floor(){}

    Floor(double width, double tile){
        reference_point = Point(-width/2, -width/2, 0);
        tileCount = ceil(width / tile);
        tileWidth = tile;
    }

    void draw(){
        glPushMatrix();

        int halfTile = tileCount / 2;
        for(int i = -halfTile; i < halfTile; i++){
            for(int j= -halfTile; j < halfTile; j++){
                if((i+j)%2) glColor3f(0,0,0);
                else glColor3f(1,1,1);
                glBegin(GL_QUADS);{
                    glVertex3f(tileWidth*i, tileWidth*j, 0);
                    glVertex3f(tileWidth*(i+1), tileWidth*j, 0);
                    glVertex3f(tileWidth*(i+1), tileWidth*(j+1), 0);
                    glVertex3f(tileWidth*i, tileWidth*(j+1), 0);
                }glEnd();
            }
        }

        glPopMatrix();
    }
};



class Light {
public:
    Point light_pos;
    Color color;

    Light(){};

    virtual void draw(){
        // this function will be overridden
    }

    void setPosition(Point &pos){
        light_pos = pos;
    }

    void setColor(Color &color){
        this->color = color;
        this->color.Normalize();
    }

    void setColor(double R, double G, double B){
        color.r = R;
        color.g = G;
        color.b = B;
        this->color.Normalize();
    }
};

class PointLight : public Light {
public:
    PointLight(Point &pos, Color &c){
        light_pos = pos;
        color = c;
        color.Normalize();
    }

    PointLight(Point &pos){
        light_pos = pos;
    }

    PointLight(){}

    void draw(){
        glPushMatrix();

        glColor3f(color.r, color.g, color.b);
        glBegin(GL_POINTS); {
            glVertex3f(light_pos.x, light_pos.y, light_pos.z);
        }glEnd();

        glPopMatrix();
    }

};

class SpotLight : public Light {
public:
    Vector direction;
    double cutoffAngle;

    SpotLight(){
        cutoffAngle = 0;
    }

    SpotLight(Point &pos, Color &c, Vector &dir, double coa=0.0){
        light_pos = pos;
        color = c;
        direction = dir;
        cutoffAngle = coa;
        color.Normalize();
    }

    SpotLight(Vector &dir, double coa) {
        direction = dir;
        cutoffAngle = coa;
    }

    void draw(){
        glPushMatrix();

        glColor3f(color.r, color.g, color.b);
        glBegin(GL_POINTS); {
            glVertex3f(light_pos.x, light_pos.y, light_pos.z);
        }glEnd();

        glPopMatrix();
    }

    void setDirection(Vector &dir){
        direction = dir;
    }

    void setCutOffAngle(double coa){
        cutoffAngle = coa;
    }
};



class Ray {
public:
    Point start;
    Vector dir;

    Ray(Point &start, Vector dir){
        this->start = start;
        this->dir = dir;
        this->dir.Normalize();
    }
};





















































