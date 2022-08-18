#include "1705050_classes.h"
#include "bitmap_image.hpp"



double cameraHeight;
double cameraAngle;
int drawgrid;
int drawaxes;
double angle;

double rotationUnit = 0.035;
double movementUnit = 2.0;

Point pos, u, r, l;


vector<Object*> objects;
vector<PointLight*> pointLights;
vector<SpotLight*> spotLights;

int recursion_level;
int dimension;


int windowWidth = 500, windowHeight = 500;
double viewAngle = 80;


void drawAxes(){
	if(drawaxes==1)
	{
		glColor3f(1.0, 1.0, 1.0);
		glBegin(GL_LINES);{
			glVertex3f( 100,0,0);
			glVertex3f(-100,0,0);

			glVertex3f(0,-100,0);
			glVertex3f(0, 100,0);

			glVertex3f(0,0, 100);
			glVertex3f(0,0,-100);
		}glEnd();
	}
}


void drawGrid(){
	int i;
	if(drawgrid==1)
	{
		glColor3f(0.6, 0.6, 0.6);	//grey
		glBegin(GL_LINES);{
			for(i=-8;i<=8;i++){

				if(i==0)
					continue;	//SKIP the MAIN axes

				//lines parallel to Y-axis
				glVertex3f(i*10, -90, 0);
				glVertex3f(i*10,  90, 0);

				//lines parallel to X-axis
				glVertex3f(-90, i*10, 0);
				glVertex3f( 90, i*10, 0);
			}
		}glEnd();
	}
}

void drawSquare(double a){
    glColor3f(1.0,0.0,0.0);
    a = a / 2.0;
	glBegin(GL_QUADS);{
		glVertex3f(0,a, a);
		glVertex3f(0,a, -a);
		glVertex3f(0,-a, -a);
		glVertex3f(0,-a, a);
	}glEnd();
}

void drawSphere(double radius,int slices,int stacks){
	struct Point points[100][100];
	int i,j;
	double h,r;
	//generate points
	for(i=0;i<=stacks;i++)
	{
		h=radius*sin(((double)i/(double)stacks)*(pi/2));
		r=radius*cos(((double)i/(double)stacks)*(pi/2));
		for(j=0;j<=slices;j++)
		{
			points[i][j].x=r*cos(((double)j/(double)slices)*2*pi);
			points[i][j].y=r*sin(((double)j/(double)slices)*2*pi);
			points[i][j].z=h;
		}
	}
	//draw quads using generated points
	for(i=0;i<stacks;i++)
	{
        glColor3f((double)i/(double)stacks,(double)i/(double)stacks,(double)i/(double)stacks);
		for(j=0;j<slices;j++)
		{
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
}


/// Task 1 Start

// Camera Position

void moveForward(){
    pos.x += l.x;
    pos.y += l.y;
    pos.z += l.z;
}

void moveBackward(){
    pos.x -= l.x;
    pos.y -= l.y;
    pos.z -= l.z;
}

void moveRight(){
    pos.x += r.x;
    pos.y += r.y;
    pos.z += r.z;
}

void moveLeft(){
    pos.x -= r.x;
    pos.y -= r.y;
    pos.z -= r.z;
}

void pageUp(){
    pos.x += u.x;
    pos.y += u.y;
    pos.z += u.z;
}

void pageDown(){
    pos.x -= u.x;
    pos.y -= u.y;
    pos.z -= u.z;
}

// Rotations

Point rotateVectorCCW(Point vect, Point helper, double sign = 1.0){
    double rotationSinA = sin(rotationUnit);
    double rotationCosA = cos(rotationUnit);

    Point temp;
    // perp(vect) is helper
    Point perpVectsinA;
    Point vectCosA;

    perpVectsinA.x = helper.x * rotationSinA * sign;
    perpVectsinA.y = helper.y * rotationSinA * sign;
    perpVectsinA.z = helper.z * rotationSinA * sign;

    vectCosA.x = vect.x * rotationCosA;
    vectCosA.y = vect.y * rotationCosA;
    vectCosA.z = vect.z * rotationCosA;

    temp.x = perpVectsinA.x + vectCosA.x;
    temp.y = perpVectsinA.y + vectCosA.y;
    temp.z = perpVectsinA.z + vectCosA.z;

    return temp;
}

Point rotateVectorCW(Point vect, Point helper, double sign = 1.0){
    double rotationSinA = sin(-rotationUnit);
    double rotationCosA = cos(-rotationUnit);
    Point temp;
    // perp(vect) is helper
    Point perpVectsinA;
    Point vectCosA;

    perpVectsinA.x = helper.x * rotationSinA * sign;
    perpVectsinA.y = helper.y * rotationSinA * sign;
    perpVectsinA.z = helper.z * rotationSinA * sign;

    vectCosA.x = vect.x * rotationCosA;
    vectCosA.y = vect.y * rotationCosA;
    vectCosA.z = vect.z * rotationCosA;

    temp.x = perpVectsinA.x + vectCosA.x;
    temp.y = perpVectsinA.y + vectCosA.y;
    temp.z = perpVectsinA.z + vectCosA.z;

    return temp;
}

void lookLeft(){
    Point tempR = rotateVectorCCW(r,l);
    l = rotateVectorCCW(l,r,-1.0);
    r = tempR;
}

void lookRight(){
    Point tempL = rotateVectorCW(l,r,-1.0);
    r = rotateVectorCW(r,l);
    l = tempL;
}

void lookUp(){
    Point tempL = rotateVectorCCW(l,u);
    u = rotateVectorCCW(u,l,-1.0);
    l = tempL;
}

void lookDown(){
    Point tempU = rotateVectorCW(u,l,-1.0);
    l = rotateVectorCW(l,u);
    u = tempU;
}

void tiltCCW(){
    Point tempU = rotateVectorCCW(u,r);
    r = rotateVectorCCW(r,u,-1.0);
    u = tempU;
}

void tiltCW(){
    Point tempR = rotateVectorCW(r,u,-1.0);
    u = rotateVectorCW(u,r);
    r = tempR;
}

/// Task 1 End

///Input
void loadData(){
    ifstream input;
    input.open("scene.txt");

    if(!input){
        cout<<"Failed to load scene.txt file"<<endl;
    }

    input >> recursion_level >> dimension;

    int objectCount;
    input >> objectCount;

    string shape;
    double x,y,z,R,G,B;
    double c1,c2,c3,c4;
    double radius, height, width, length;
    int shininess;

    while(objectCount--){
        input >> shape;

        if(shape == "sphere"){
            input >> x >> y >> z;
            Point center(x,y,z);
            input >> radius;
            Object *ob = new Sphere(center, radius);

            input >> R >> G >> B;
            ob->setColor(Color(R,G,B));

            input >> c1 >> c2 >> c3 >> c4;
            ob->setCoefficients(c1, c2, c3, c4);

            input >> shininess;
            ob->setShine(shininess);

            objects.push_back(ob);
        }
        else if(shape == "triangle"){
            Point a, b, c;

            input >> a.x >> a.y >> a.z;
            input >> b.x >> b.y >> b.z;
            input >> c.x >> c.y >> c.z;

            Object *ob = new Triangle(a,b,c);

            input >> R >> G >> B;
            ob->setColor(Color(R,G,B));

            input >> c1 >> c2 >> c3 >> c4;
            ob->setCoefficients(c1, c2, c3, c4);

            input >> shininess;
            ob->setShine(shininess);

            objects.push_back(ob);
        }

        else if(shape == "general"){
            double a,b,c,d,e,f,g,h,i,j;
            input >> a >> b >> c >> d >> e;
            input >> f >> g >> h >> i >> j;

            input >> x >> y >> z;
            Point pos(x, y, z);

            input >> length >> width >> height;

            Object *ob = new GeneralShape(a,b,c,d,e,f,g,h,i,j,length,width,height,pos);

            input >> R >> G >> B;
            ob->setColor(Color(R,G,B));

            input >> c1 >> c2 >> c3 >> c4;
            ob->setCoefficients(c1, c2, c3, c4);

            input >> shininess;
            ob->setShine(shininess);

            objects.push_back(ob);
        }

    }

    int pointLightCount;
    input >> pointLightCount;

    while(pointLightCount--){
        input >> x >> y >> z;
        Point pos(x,y,z);

        input >> R >> G >> B;
        Color color(R,G,B);

        PointLight *light = new PointLight(pos, color);
        pointLights.push_back(light);
    }

    int spotLightCount;
    input >> spotLightCount;

    while(spotLightCount--){
        input >> x >> y >> z;
        Point pos(x,y,z);

        input >> R >> G >> B;
        Color color(R,G,B);

        input >> x >> y >> z;
        Vector dir(x,y,z);

        double coa;
        input >> coa;

        SpotLight *light = new SpotLight(pos,color,dir,coa);
        spotLights.push_back(light);
    }

    Object *ob = new Floor(1000,20);
    ob->setCoefficients(0.5,0.5,0.5,0.5);
    ob->setShine(10);
    objects.push_back(ob);
}

///End Input

/// Capture Method. Will be called when 0 will be pressed
void capture(){
    // initialize bitmap image and set background color
    bitmap_image image(dimension, dimension);
    for(int i=0;i<dimension; i++){
        for(int j=0;j<dimension; j++){
            image.set_pixel(i,j,0,0,0);
        }
    }

    // planeDistance = (windowHeight/2.0) / tan(viewAngle/2.0)
    double planeDistance = (0.5*windowHeight) / tan(viewAngle * pi / 360.0); // 360 = 2*180

    //topleft = eye + l*planeDistance - r*windowWidth/2 + u*windowHeight/2
    Point topLeft;
    topLeft.x = pos.x + l.x*planeDistance - r.x * windowWidth/2 + u.x * windowHeight/2;
    topLeft.y = pos.y + l.y*planeDistance - r.y * windowWidth/2 + u.y * windowHeight/2;
    topLeft.z = pos.z + l.z*planeDistance - r.z * windowWidth/2 + u.z * windowHeight/2;

    //du = windowWidth/imageWidth
    double du = (double)windowWidth / dimension;

    //dv = windowHeight/imageHeight
    double dv = (double)windowHeight / dimension;

    // Choose middle of the grid cell
    // topleft = topleft + r*(0.5*du) - u*(0.5*dv)
    topLeft.x = topLeft.x + r.x * (0.5*du) - u.x * (0.5*dv);
    topLeft.y = topLeft.y + r.y * (0.5*du) - u.y * (0.5*dv);
    topLeft.z = topLeft.z + r.z * (0.5*du) - u.z * (0.5*dv);

    int nearest;
    double t, tMin;
    Point curPixel;

    // for i=1:imageWidth
    for(int i=0; i<dimension; i++){
        // for j=1:imageHeight
        for(int j=0; j<dimension; j++){
            // calculate curPixel using topleft,r,u,i,j,du,dv
            curPixel.x = topLeft.x + r.x * (du * i) - u.x * (j * dv);
            curPixel.y = topLeft.y + r.y * (du * i) - u.y * (j * dv);
            curPixel.z = topLeft.z + r.z * (du * i) - u.z * (j * dv);

            // cast ray from eye to (curPixel-eye) direction
            Vector dir;
            dir.x = curPixel.x - pos.x;
            dir.y = curPixel.y - pos.y;
            dir.z = curPixel.z - pos.z;
            Ray ray(pos, dir);

            Color color;

            // for each object, o in objects
            for(Object* o: objects){
                ;
                /// will be done later
                /// after completing intersect method
            }

        }
    }

}


void keyboardListener(unsigned char key, int x,int y){
	switch(key){

		case '1':
			lookLeft();
			break;
        case '2':
            lookRight();
            break;
        case '3':
            lookUp();
            break;
        case '4':
            lookDown();
            break;
        case '5':
            tiltCCW();
            break;
        case '6':
            tiltCW();
            break;
        case '0':
            capture();
            break;


		default:
			break;
	}
}

void specialKeyListener(int key, int x,int y){
	switch(key){
		case GLUT_KEY_DOWN:		//down arrow key
			moveBackward();
			break;
		case GLUT_KEY_UP:		// up arrow key
			moveForward();
			break;

		case GLUT_KEY_RIGHT:
			moveRight();
			break;
		case GLUT_KEY_LEFT:
			moveLeft();
			break;

		case GLUT_KEY_PAGE_UP:
		    pageUp();
			break;
		case GLUT_KEY_PAGE_DOWN:
		    pageDown();
			break;

		case GLUT_KEY_INSERT:
			break;

		case GLUT_KEY_HOME:
			break;
		case GLUT_KEY_END:
			break;

		default:
			break;
	}
}

void mouseListener(int button, int state, int x, int y){	//x, y is the x-y of the screen (2D)
	switch(button){
		case GLUT_LEFT_BUTTON:
			if(state == GLUT_DOWN){		// 2 times?? in ONE click? -- solution is checking DOWN or UP
				drawaxes=1-drawaxes;
			}
			break;

		case GLUT_RIGHT_BUTTON:
		    if(state == GLUT_UP){
                drawgrid = 1 - drawgrid;
		    }
			//........
			break;

		case GLUT_MIDDLE_BUTTON:
			//........
			break;

		default:
			break;
	}
}

void display(){

	//clear the display
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0,0,0,0);	//color black
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/********************
	/ set-up camera here
	********************/
	//load the correct matrix -- MODEL-VIEW matrix
	glMatrixMode(GL_MODELVIEW);

	//initialize the matrix
	glLoadIdentity();

	//now give three info
	//1. where is the camera (viewer)?
	//2. where is the camera looking?
	//3. Which direction is the camera's UP direction?

	//gluLookAt(100,100,100,	0,0,0,	0,0,1);
	//gluLookAt(80*cos(cameraAngle), 80*sin(cameraAngle), cameraHeight,		0,0,0,		0,0,1);
	//gluLookAt(0,0,200,	0,0,0,	0,1,0);
	gluLookAt(
           pos.x, pos.y, pos.z,
           pos.x + l.x, pos.y + l.y, pos.z + l.z,
           u.x, u.y, u.z
           );


	//again select MODEL-VIEW
	glMatrixMode(GL_MODELVIEW);


	/****************************
	/ Add your objects from here
	****************************/
	//add objects

	drawAxes();
	drawGrid();

    glColor3f(1,0,0);
    drawSquare(100);

	//drawSphere(50,50,50);


	//ADD this line in the end --- if you use double buffer (i.e. GL_DOUBLE)
	glutSwapBuffers();
}


void animate(){
	angle+=0.05;
	//codes for any changes in Models, Camera
	glutPostRedisplay();
}

void init(){
	//codes for initialization
	drawgrid=0;
	drawaxes=1;
	cameraHeight=150.0;
	cameraAngle=1.0;
	angle=0;

	pos.x = 100;
	pos.y = 100;
	pos.z = 0;

	u.x = 0;
	u.y = 0;
	u.z = 1;

	r.x = - 1/sqrt(2.0);
	r.y = 1/sqrt(2.0);
	r.z = 0;

	l.x = - 1/sqrt(2.0);
	l.y = - 1/sqrt(2.0);
	l.z = 0;

	//clear the screen
	glClearColor(0,0,0,0);

	/************************
	/ set-up projection here
	************************/
	//load the PROJECTION matrix
	glMatrixMode(GL_PROJECTION);

	//initialize the matrix
	glLoadIdentity();

	//give PERSPECTIVE parameters
	gluPerspective(80,	1,	1,	1000.0);
	//field of view in the Y (vertically)
	//aspect ratio that determines the field of view in the X direction (horizontally)
	//near distance
	//far distance
}

int main(int argc, char **argv){
	glutInit(&argc,argv);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(0, 0);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB);	//Depth, Double buffer, RGB color

	glutCreateWindow("My OpenGL Program");

	init();

	glEnable(GL_DEPTH_TEST);	//enable Depth Testing

	glutDisplayFunc(display);	//display callback function
	glutIdleFunc(animate);		//what you want to do in the idle time (when no drawing is occuring)

	glutKeyboardFunc(keyboardListener);
	glutSpecialFunc(specialKeyListener);
	glutMouseFunc(mouseListener);

	glutMainLoop();		//The main loop of OpenGL

	return 0;
}
