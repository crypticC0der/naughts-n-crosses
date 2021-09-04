#include <GL/freeglut_std.h>
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
int brainSize=2;
int winposA;
int winposB;
int calculations=2;
int gridSize;
bool botStart=false;
bool botTurn=false;

enum gameConditions{
	win=1,
	loose=-2,
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

void drawString(float x, float y, char *string) {
	glRasterPos2f(x,y);
	for (char* c = string; *c != '\0'; c++) {
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *c);  // Updates the position
	}
}

void drawChar(float x, float y, char c,void* bitmap) {
	glRasterPos2f(x,y);
	glutBitmapCharacter(bitmap, c);  // Updates the position
}

void drawChar(float x, float y, char c) {
	glRasterPos2f(x,y);
	glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, c);  // Updates the position
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
		drawChar(a,b,fillchars[grid[i]]);
		glEnd();
	}

	drawString(0.1,0.9,(char*)"\'s turn");
	drawChar(-0.15f,0.9f,fillchars[botTurn + 1],GLUT_BITMAP_HELVETICA_12);
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
				winposA=i*gridLen +j;
				//vertical
				while(i+k<gridLen && i+rowLen<=gridLen){
					if(cgrid[(i+k)*gridLen +j]==value){
						win++;
						k++;
						winposB=(i+k)*gridLen +j;
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
						winposB=(i)*gridLen +(j+k);
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
						winposB=(i+k)*gridLen + (j+k);
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
						winposB=(i+k)*gridLen + (j-k);
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

float boardCalc(filler* cgrid,filler side,filler enemy,int distance){
	//check if game over
	gameConditions cond = intWon(cgrid,side,enemy);
	if(cond!=notover){
		if (distance==0){
			return (float)cond* 500;
		}
		return (float)cond* 5/distance;
	}
	if(distance>brainSize){
		return (float)draw* 5/distance;
	}
	if (calculations%1000000==0){
	}
	calculations+=1;

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
				if(distance==0){
					return cond*100;
				}else{
					return (float)cond * 5/distance;
				}
			}

			//try for enempy moves
			for(int j =0;j<gridSize;j++){
				if(cgrid[j]==space){
					cgrid[j]=side;
					trySum+=boardCalc(cgrid,side,enemy,distance+1);
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
			float out = boardCalc(cgrid,side,enemy,0);
			if(out>=maxscore){
				maxpos=i;
				maxscore=out;
			}
			cgrid[i]=space;
		}
	}
	cgrid[maxpos]=side;
}

void run(){
	filler winner;
	if(hasWon(grid,&winner)){
		for(int i =0;i<gridSize;i++){
			grid[i]=space;
		}

		if(winner!=space){
			//draw line showing winning combo
			cout<<winposA<<","<<winposB<<endl;
			glColor3f(1,0,0);
			glBegin(GL_LINES);
			float halfSq=1/(float)(gridLen);
			float x,y;
			x=winposB%gridLen;
			y=(winposB/gridLen);
			x/=gridLen;
			y/=gridLen;
			x=x*2-1;
			y=y*2-1;
			glVertex2d(x+halfSq,y+halfSq);
			cout << x <<" , " <<y  <<endl;

			x=(winposA%gridLen);
			y=(winposA/gridLen);
			x/=gridLen;
			y/=gridLen;
			x=x*2-1;
			y=y*2-1;
			glVertex2d(x+halfSq,y+halfSq);
			glEnd();
			glFlush();
			cout << x <<" , " <<y  <<endl;
		}
		
		botTurn=true;
		sleep(2);
		disInit();
		botTurn=!botStart;
		botStart=botTurn;
	}
	if (botTurn){
		move(grid,cross,naught);
		botTurn=false;
		disInit();
	}
}

void clickdy(int button,int state,int x,int y){
	int i= gridLen * x/300; 
	int j= gridLen - (1 + gridLen * y/300);
	if(!botTurn && button==GLUT_LEFT_BUTTON && state==GLUT_DOWN){
		grid[j*gridLen + i]=naught;
		botTurn=true;
		disInit();
	}
}

int main(int argc, char** argv) {
	srand (time(NULL));

	if(gridLen<4){
		brainSize=10;
	}else if(gridLen>10){
		brainSize=0;
	}else if(gridLen>5){
		brainSize=1;
	}

	gridSize=gridLen*gridLen;
	//while(!generateGrid()){}
	filler* cgrid=new filler[gridSize];
	for(int i =0;i<gridSize;i++){
		cgrid[i]=space;
	}
	grid=cgrid;
	glutInit(&argc, argv);		// Initialize GLUT
    glutInitWindowSize(300,300);   // Set the window's initial width & height
    glutCreateWindow("Sudoku"); // Create a window with the given title
    glutInitWindowPosition(50, 50); // Position the window's initial top-left corner
    glutDisplayFunc(disInit); // Register display callback handler for window re-paint
	glutMouseFunc(clickdy);
	glutIdleFunc(run);
	glutMainLoop();
	return 0;
}
