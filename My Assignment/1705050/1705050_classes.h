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
class Ray;

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
    Vector(Point p, Point q){
        this->x = q.x - p.x;
        this->y = q.y - p.y;
        this->z = q.z - p.z;
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
        r=g=b=0.0;
    }
    Color(double R, double G, double B){
        r = R;
        g = G;
        b = B;
    }

    void setColor(Color c){
        r = c.r;
        g = c.g;
        b = c.b;
    }

    void Scale(double s){
        r *= s;
        g *= s;
        b *= s;
        Normalize();
    }

    void Normalize(){
        if(r < 0) r = 0.0;
        if(r > 1) r = 1.0;
        if(g < 0) g = 0.0;
        if(g > 1) g = 1.0;
        if(b < 0) b = 0.0;
        if(b > 1) b = 1.0;

//        double M = 1.0;
//        if(r > M){
//            M = r;
//        }
//        if(g > M){
//            M = g;
//        }
//        if(b > M){
//            M = b;
//        }
//        r /= M;
//        g /= M;
//        b /= M;

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




class Object {
public:
    Point reference_point;
    double height, width, length;
    Color color;
    double coefficients[4];
    // 0 -> ambient coefficients
    // 1 -> diffuse coefficients
    // 2 -> specular coefficients
    // 3 -> reflection coefficients
    int shine; // exponent term of specular component

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

    virtual double intersect(Ray *r, Color *colorI, int level){
        return -1.0;
    }

    double intersectionPhongModel(Ray *r, Color *colorI, int level){
        double tMin = intersect(r,colorI, level);

        // if level is 0, return tmin
        if(level == 0){
            return tMin;
        }

        Vector ro(Point(), r->start);
        Vector dir = r->dir;

        // intersectionPoint = r->start + r->dir*tmin
        Point intersectionPoint;
        intersectionPoint.x = ro.x + dir.x * tMin;
        intersectionPoint.y = ro.y + dir.y * tMin;
        intersectionPoint.z = ro.z + dir.z * tMin;

        // color = intersectionPointColor*coEfficient[AMB]
        Color newColor = getIntersectionPointColor(intersectionPoint);
        newColor.Scale(coefficients[AMBIENT]);

        // calculate normal at intersectionPoint
        Vector normal = this->calculateNormal(intersectionPoint);

        // for each point light pl in pointLights
        for(PointLight *light: pointLights){
            // cast rayl from pl.light_pos to intersectionPoint
            Point lightPos = light->light_pos;
            Vector rayDir = Vector(intersectionPoint, lightPos );
            rayDir.Normalize();

            Ray *lightRay = new Ray(intersectionPoint, rayDir);

            // if intersectionPoint is in shadow, the diffuse
            // and specular components need not be calculated
            // for shadow, other object needed between light and object
            bool shadow = false;
            double t, tMin2;
            tMin2 = INFINITY;
            Color *dummy = new Color();

            for(Object *o: objects){
                t = o->intersect(lightRay,dummy, 0);
                if(t > 0 && t < tMin2){
                    tMin2 = t;
                }
            }

//            if(tMin > tMin2){ // another object in between
//                shadow = true;
//            }

            Point n;
            n.x = ro.x + dir.x * tMin;
            n.y = ro.y + dir.y * tMin;
            n.z = ro.z + dir.z * tMin;

            Point m = intersectionPoint;

            double dist1 = sqrt(pow(m.x-ro.x, 2) + pow(m.y-ro.y, 2) + pow(m.z-ro.z, 2));
            double dist2 = sqrt(pow(n.x-ro.x, 2) + pow(n.y-ro.y, 2) + pow(n.z-ro.z, 2));

            if((dist1 - 0.0000001) > dist2) shadow = true;

            if(!shadow){
                // calculate lambertValue using normal, rayl
                double dotValue = Vector::DotProduct(normal,rayDir);
                double lambertValue;
                if(dotValue < 0.0) lambertValue = 0.0;
                else lambertValue = dotValue;

                // find reflected ray, rayr for rayl
                double multiplier = dotValue * 2.0;
                Vector reflection;
                reflection.x = normal.x * multiplier - rayDir.x;
                reflection.y = normal.y * multiplier - rayDir.y;
                reflection.z = normal.z * multiplier - rayDir.z;

                reflection.Normalize();

                // calculate phongValue using r, rayr
                double phongValue = Vector::DotProduct(dir, reflection);
                if(phongValue < 0.0) phongValue = 0.0;

                double phongShine = pow(phongValue, shine);

                // color += pl.color*coEfficient[DIFF]*lambertValue*intersectionPointColor
                newColor.r += light->color.r * coefficients[DIFFUSE] * lambertValue * getIntersectionPointColor(intersectionPoint).r;
                newColor.g += light->color.g * coefficients[DIFFUSE] * lambertValue * getIntersectionPointColor(intersectionPoint).g;
                newColor.b += light->color.b * coefficients[DIFFUSE] * lambertValue * getIntersectionPointColor(intersectionPoint).b;
                newColor.Normalize();
                // color+= pl.color*coEfficient[SPEC]*phongValueshine * intersectionPointColor
                newColor.r += light->color.r * coefficients[SPECULAR] * phongShine * getIntersectionPointColor(intersectionPoint).r;
                newColor.g += light->color.g * coefficients[SPECULAR] * phongShine * getIntersectionPointColor(intersectionPoint).g;
                newColor.b += light->color.b * coefficients[SPECULAR] * phongShine * getIntersectionPointColor(intersectionPoint).b;

                newColor.Normalize();
            }

        }

        colorI->setColor(newColor);
        return tMin;
    }

    virtual Vector calculateNormal(Point &p){
        return Vector();
    }

    virtual Color getIntersectionPointColor(Point &p){
        return color;
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
    double radius;

    Sphere(){
        reference_point = Point();
        radius = 0;
    }

    Sphere(Point center, double radius){
        reference_point = center;
        this->radius = radius;
    }

    double intersect(Ray *r, Color *colorI, int level){
        /// ref : Slide-30
        Vector ro;
        ro.x = r->start.x - reference_point.x;
        ro.y = r->start.y - reference_point.y;
        ro.z = r->start.z - reference_point.z;

        Vector dir = r->dir;
        double b = 2.0 * Vector::DotProduct(dir, ro);
        double c = Vector::DotProduct(ro, ro) - radius * radius;
        // a = 1
        double d = b * b - 4.0 * c;
        if(d < 0){
            return -1;
        }
        d = sqrt(d);
        double tPlus = (- b + d) / 2.0;
        double tMinus = (-b - d) / 2.0;

        Color newColor(color);


        if(tMinus > 0) {
            colorI->setColor(newColor);
            return tMinus;
        }
        else if(tPlus > 0) {
            colorI->setColor(newColor);
            return tPlus;
        }
        else return -1;
    }

    void draw() {
        glPushMatrix();
        glTranslatef(reference_point.x, reference_point.y, reference_point.z);
        struct Point points[100][100];
        int i,j;
        double h,r;
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
            glColor3f(color.r, color.g, color.b);
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

    Vector calculateNormal(Point &p){
        Vector n(reference_point, p);
        n.Normalize();
        return n;
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

    double intersect(Ray *r, Color *colorI, int level){
        /// ref : Slide
        Vector ro(Point(), r->start);
        Vector dir = r->dir;

        Vector ab(points[0], points[1]);
        Vector ac(points[0], points[2]);

        Vector temp = Vector::CrossProduct(dir, ac);
        double alpha = Vector::DotProduct(ab, temp);

        // alpha will be zero if ray is parallel to the triangle plane
        if(abs(alpha) < 0.0000001){
            return -1;
        }

        Vector ao(points[0], r->start);
        double beta = Vector::DotProduct(ao, temp) / alpha;

        // value of beta must be in range [0,1]
        if(beta < 0 || beta > 1.0) {
            return -1;
        }

        temp = Vector::CrossProduct(ao, ab);
        double gama = Vector::DotProduct(dir, temp) / alpha;

        // value of gama must be greater than 0
        // and beta + gama must not greater than 1
        if(gama < 0 || beta + gama > 1.0){
            return -1;
        }

        double t = Vector::DotProduct(ac, temp) / alpha;
        if(t < 0.0000001){
            return -1;
        }
        colorI->setColor(color);
        return t;
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

    Vector calculateNormal(Point &p){
        Vector ab(points[0], points[1]);
        Vector ac(points[0], points[2]);
        Vector n = Vector::CrossProduct(ab, ac);
        n.Normalize();
        return n;
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

    bool isInReferenceCube(Vector &v){
        if(v.x < reference_point.x || v.x > reference_point.x + length){
            return false;
        }

        if(v.y < reference_point.y || v.y > reference_point.y + width){
            return false;
        }

        if(v.z < reference_point.z || v.z > reference_point.z + height){
            return false;
        }
        return true;
    }

    double intersect(Ray *r, Color *colorI, int level){
        /// ref : http://skuld.bmsc.washington.edu/people/merritt/graphics/quadrics.html
        Vector ro(Point(), r->start);
        Vector dir = r->dir;

        double a = A * dir.x * dir.x + B * dir.y * dir.y + C * dir.z * dir.z
                    + D * dir.x * dir.y + E * dir.x * dir.z + F * dir.y * dir.z;

        double b = 2.0 * (A * ro.x * dir.x + B * ro.y * dir.y + C * ro.z * dir.z )
                    + D * (ro.x * dir.y + ro.y * dir.x) + E * (ro.x * dir.z + ro.z * dir.x)
                    + F * (ro.y * dir.z + ro.z * dir.y) + G * dir.x + H * dir.y + I * dir.z;

        double c = A * ro.x * ro.x + B * ro.y * ro.y + C * ro.z * ro.z + D * ro.x * ro.y
                    + E * ro.x * ro.z + F * ro.y * ro.z + G * ro.x + H * ro.y + I * ro.z + J;

        double d = b * b - 4.0 * a * c;

        if(d < 0){
            return -1;
        }

        d = sqrt(d);

        double tPlus = (- b + d)/(2.0 * a);
        double tMinus = (- b - d)/(2.0 * a);

        Vector pPlus, pMinus;

        pPlus.x = ro.x + dir.x * tPlus;
        pPlus.y = ro.y + dir.y * tPlus;
        pPlus.z = ro.z + dir.z * tPlus;

        pMinus.x = ro.x + dir.x * tMinus;
        pMinus.y = ro.y + dir.y * tMinus;
        pMinus.z = ro.z + dir.z * tMinus;

        if(tMinus > 0 && isInReferenceCube(pMinus)) {
            colorI->setColor(color);
            return tMinus;
        }
        else if(tPlus > 0 && isInReferenceCube(pPlus)) {
            colorI->setColor(color);
            return tPlus;
        }
        else return -1;
    }

    void draw(){
        glPushMatrix();
        // no need to draw this

        glPopMatrix();
    }

    Vector calculateNormal(Point &p){
        double nx = 2.0 * A * p.x + D * p.y + E * p.z + G;
        double ny = 2.0 * B * p.y + D * p.x + F * p.z + H;
        double nz = 2.0 * C * p.z + E * p.x + F * p.y + I;
        Vector n(nx,ny,nz);
        n.Normalize();
        return n;
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

    bool isInReferenceRegion(Point p){
        if(p.x < reference_point.x || p.x > -reference_point.x){
            return false;
        }
        if(p.y < reference_point.y || p.y > -reference_point.y){
            return false;
        }
        return true;
    }

    Color getIntersectionPointColor(Point &p){
        double start = (double)(-tileCount*tileWidth/2);
        int i = (int)((p.x - start)/tileWidth);
        int j = (int)((p.y - start)/tileWidth);
        if((i+j)%2){
            return Color(0.0,0.0,0.0);
        }
        else {
            return Color(1.0,1.0,1.0);
        }
    }

    double intersect(Ray *r, Color *colorI, int level){
        Vector ro(Point(), r->start);
        Vector dir = r->dir;
        Vector n(0.0, 0.0, 1.0);

        double DotRo = Vector::DotProduct(n, ro);
        double DotDir = Vector::DotProduct(n, dir);

        double t = - DotRo/DotDir;
        Point intersectingPoint;
        intersectingPoint.x = ro.x + dir.x * t;
        intersectingPoint.y = ro.y + dir.y * t;
        intersectingPoint.z = ro.z + dir.z * t;

        if(isInReferenceRegion(intersectingPoint) && t > 0){
            colorI->setColor(getIntersectionPointColor(intersectingPoint));
            return t;
        }

        return -1;
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

    Vector calculateNormal(Point &p){
        return Vector(0.0,0.0,1.0);
    }
};

























































