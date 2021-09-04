#include <iostream>
#include <fstream>
#include <GL/glut.h>
#include <GL/freeglut.h>
#include <time.h> 
#include <stdlib.h>
#include <unistd.h>
using namespace std;

const int gridLen =4;
int gridSize;
int* grid;

void drawString(float x, float y, char *string) {
	glRasterPos2f(x,y);
	for (char* c = string; *c != '\0'; c++) {
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *c);  // Updates the position
	}
}

void DrawGrid(){
	glColor3f(0.8f,0.8f,0.8f);
	glBegin(GL_QUADS);
	float halfSq = (float)(2)/(gridLen);
	for(int i=0;i<gridLen;i++){
		for(int j=i%2;j<gridLen;j+=2){
			float a = (float)(i<<1)/gridLen -1;
			float b = (float)(j<<1)/gridLen -1;
			glVertex2d(a,b);
			glVertex2d(a+halfSq,b);
			glVertex2d(a+halfSq,b+halfSq);
			glVertex2d(a,b+halfSq);
		}
	}
	glEnd();
	glColor3f(0,0,0);
	glBegin(GL_QUADS);

	glEnd();
	glColor3f(0,0,0);
	glBegin(GL_LINES);
	for(int i =-gridLen+2;i<gridLen;i+=2){
		float k = (float)(i)/gridLen;
		glVertex2d(1,k);
		glVertex2d(-1,k);
		glVertex2d(k,1);
	  	glVertex2d(k,-1);
	}
	glEnd();
}

void disInit(){
	glClearColor(1, 1, 1, 0); // Set background color to black and opaque
	glClear(GL_COLOR_BUFFER_BIT);         // Clear the color buffer
	DrawGrid();
	//nextGeneration();
	glFlush();
}



void run(){
}

int main(int argc, char** argv) {
	srand (time(NULL));
	gridSize=gridLen*gridLen;
	//while(!generateGrid()){}
	int* cgrid=new int[gridSize];
	
	//read grid from file
	glutInit(&argc, argv);		// Initialize GLUT
    glutInitWindowSize(600,600);   // Set the window's initial width & height
    glutCreateWindow("Sudoku"); // Create a window with the given title
    glutInitWindowPosition(50, 50); // Position the window's initial top-left corner
    glutDisplayFunc(disInit); // Register display callback handler for window re-paint
	glutIdleFunc(run);
	glutMainLoop();
	return 0;
}
