#include <iostream>
#include <fstream>
#include <GL/glut.h>
#include <GL/freeglut.h>
#include <time.h> 
#include <stdlib.h>
#include <unistd.h>
using namespace std;

const int gridLen =4;
const int rowLen =4; //number in a row to win
int gridSize;

enum gameConditions{
	win=1,
	loose=-1,
	draw=0,
	notover=404
};

enum filler{
	space,
	naught,
	cross,
	block
};

char fillchars[] =  {' ','o','x','/'};

filler* grid; //yes im well aware im wasting bits here but fuck you, i cant be asked to code a better solution

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

bool hasWon(filler* cgrid,filler* win){
	bool draw=true;
	for(int i=0;i<gridLen;i++){
		for(int j=0;j<gridLen;j++){
			filler value = cgrid[i*gridLen + j];
			draw = draw&&value!=space;
			if(value==naught||value==cross){
				*win=value;
				int win=1;
				int k=1;
				//vertical
				while(i+k<gridLen && i+rowLen<=gridLen){
					if(cgrid[(i+k)*gridLen +j]==value){
						win++;
						k++;
					}else{
						break;
					}
				}
				if(win>=rowLen){
					return true;
				}

				win=1;
				k=1;
				//horiz
				while(j+k<gridLen && j+rowLen<=gridLen){
					if(cgrid[i*gridLen +(j+k)]==value){
						win++;
						k++;
					}else{
						break;
					}
				}
				if(win>=rowLen){
					return true;
				}

				win=1;
				k=1;
				//diagonal 1
				while(j+k<gridLen && j+rowLen<=gridLen && i+k<gridLen && i+rowLen<=gridLen){
					if(cgrid[(i+k)*gridLen +(j+k)]==value){
						win++;
						k++;
					}else{
						break;
					}
				}
				if(win>=rowLen){
					return true;
				}

				win=1;
				k=1;
				//diagonal 2
				while(j-k>=0&& j-rowLen>=-1&& i+k<gridLen && i+rowLen<=gridLen){
					if(cgrid[(i+k)*gridLen +(j-k)]==value){
						win++;
						k++;
					}else{
						break;
					}
				}
				if(win>=rowLen){
					return true;
				}
			}
		}
	}
	*win=space;
	return draw;
}

gameConditions intWon(filler* cgrid,filler side,filler enemy){
	filler output;
	bool won = hasWon(cgrid,&output);
	if (won){
		if(output==side){
			return win;
		}
		else if(output==enemy){
			return loose;
		}
		else{
			return draw;
		}
	}
	return notover;
}

float moveCalc(filler* cgrid,filler side,filler enemy){
	//check if game over
	gameConditions cond = intWon(cgrid,side,enemy);
	if(cond!=notover){
		return cond;
	}

	//continue game
	int tryCount=0;
	float trySum=0;
	for(int i =0;i<gridSize;i++){
		if(cgrid[i]==space){
			//attempt
			cgrid[i]=enemy;
			
			//check game state
			gameConditions cond = intWon(cgrid,side,enemy);
			if(cond!=notover){
				cgrid[i]=space;
				return cond;
			}

			//try for enempy moves
			for(int j =0;j<gridSize;j++){
				if(cgrid[j]==space){
					cgrid[j]=side;
					trySum+=moveCalc(cgrid,side,enemy);
					tryCount+=1;
					cgrid[j]=space;
				}
			}
			cgrid[i]=space;
		}
	}
	return trySum/tryCount;
}

void move(filler* cgrid,filler side, filler enemy){
	int maxpos = 0;
	float maxscore=-10000;
	for(int i =0;i<gridSize;i++){
		if(cgrid[i]==space){
			cgrid[i]=side;
			cout << "testing " << i << endl;
			float out = moveCalc(cgrid,side,enemy);
			if(out>=maxscore){
				maxpos=i;
				maxscore=out;
			}
			cgrid[i]=space;
		}
	}
	cgrid[maxpos]=side;
	cout << maxpos<<endl;
}

void run(){
}

int main(int argc, char** argv) {
	srand (time(NULL));
	gridSize=gridLen*gridLen;
	//while(!generateGrid()){}
	filler* cgrid=new filler[gridSize];
	for(int i =0;i<gridSize;i++){
		cgrid[i]=space;
	}
	filler winner;
	filler me,enemy;
	me=cross;
	enemy=naught;
	while(!hasWon(cgrid,&winner)){
		cout << "make a move" << endl;
		move(cgrid,me,enemy);
		filler t=me;
		me=enemy;
		enemy=t;
	}
	grid=cgrid;
	cout << winner<<endl;
	
	glutInit(&argc, argv);		// Initialize GLUT
    glutInitWindowSize(300,300);   // Set the window's initial width & height
    glutCreateWindow("Sudoku"); // Create a window with the given title
    glutInitWindowPosition(50, 50); // Position the window's initial top-left corner
    glutDisplayFunc(disInit); // Register display callback handler for window re-paint
	glutIdleFunc(run);
	glutMainLoop();
	return 0;
}
