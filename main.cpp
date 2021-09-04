#include <iostream>
#include <fstream>
#include <GL/glut.h>
#include <GL/freeglut.h>
#include <time.h> 
#include <stdlib.h>
#include <unistd.h>
using namespace std;

const int gridLen =3;
int gridSize;

enum fillers{
	space,
	naught,
	cross,
	block
};

char fillchars[] =  {' ','o','x','/'};

fillers* grid; //yes im well aware im wasting bits here but fuck you, i cant be asked to code a better solution

void drawString(float x, float y, char c) {
	glRasterPos2f(x,y);
	glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);  // Updates the position
}

void DrawGrid(){
	glColor3f(0.8f,0.8f,0.8f);
	glBegin(GL_QUADS);
	float halfSq = (float)(2)/(gridLen);
	int i,j;
	float a,b;
	for(i=0;i<gridLen;i++){
		for(j=i%2;j<gridLen;j+=2){
			a = (float)(i<<1)/gridLen -1;
			b = (float)(j<<1)/gridLen -1;
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
	for(i =-gridLen+2;i<gridLen;i+=2){
		float a = (float)(i)/gridLen;
		glVertex2d(1,a);
		glVertex2d(-1,a);
		glVertex2d(a,1);
	  	glVertex2d(a,-1);
	}
	glEnd();

	//fill grid 
	for(i=0;i<gridSize;i++){
		a = (float)((i%gridLen)<<1)/gridLen -1 + halfSq/2;
		b = (float)((i/gridLen)<<1)/gridLen -1 + halfSq/2;
		drawString(a,b,fillchars[grid[i]]);
		glEnd();
		cout << i << ": " << fillchars[grid[i]] << " at " << a << "," << b << endl;
	}
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
	grid=new fillers[gridSize];
	for(int i =0;i<gridSize;i++){
		grid[i]=naught;
	}
	
	glutInit(&argc, argv);		// Initialize GLUT
    glutInitWindowSize(300,300);   // Set the window's initial width & height
    glutCreateWindow("Sudoku"); // Create a window with the given title
    glutInitWindowPosition(50, 50); // Position the window's initial top-left corner
    glutDisplayFunc(disInit); // Register display callback handler for window re-paint
	glutIdleFunc(run);
	glutMainLoop();
	return 0;
}
