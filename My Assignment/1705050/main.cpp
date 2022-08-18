#include<stdio.h>
#include<stdlib.h>
#include<math.h>

#include <windows.h>
#include <GL\glut.h>

#define pi (2*acos(0.0))

double cameraHeight;
double cameraAngle;
int drawgrid;
int drawaxes;
double angle;

double rotationUnit = 0.035;
double movementUnit = 2.0;

struct point{
	double x,y,z;
};

point pos, u, r, l;

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
	struct point points[100][100];
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

point rotateVectorCCW(point vect, point helper, double sign = 1.0){
    double rotationSinA = sin(rotationUnit);
    double rotationCosA = cos(rotationUnit);

    point temp;
    // perp(vect) is helper
    point perpVectsinA;
    point vectCosA;

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

point rotateVectorCW(point vect, point helper, double sign = 1.0){
    double rotationSinA = sin(-rotationUnit);
    double rotationCosA = cos(-rotationUnit);
    point temp;
    // perp(vect) is helper
    point perpVectsinA;
    point vectCosA;

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
    point tempR = rotateVectorCCW(r,l);
    l = rotateVectorCCW(l,r,-1.0);
    r = tempR;
}

void lookRight(){
    point tempL = rotateVectorCW(l,r,-1.0);
    r = rotateVectorCW(r,l);
    l = tempL;
}

void lookUp(){
    point tempL = rotateVectorCCW(l,u);
    u = rotateVectorCCW(u,l,-1.0);
    l = tempL;
}

void lookDown(){
    point tempU = rotateVectorCW(u,l,-1.0);
    l = rotateVectorCW(l,u);
    u = tempU;
}

void tiltCCW(){
    point tempU = rotateVectorCCW(u,r);
    r = rotateVectorCCW(r,u,-1.0);
    u = tempU;
}

void tiltCW(){
    point tempR = rotateVectorCW(r,u,-1.0);
    u = rotateVectorCW(u,r);
    r = tempR;
}

/// Task 1 End



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
