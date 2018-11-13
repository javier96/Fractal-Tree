#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <math.h>
#include <time.h>
#include <iostream>
#include <string.h>
using namespace std;
#include <GL/glut.h>
const double PI = acos(-1);

GLuint makeaTree;
float x,y,z;

/*
Fuzzy Weed
Axiom	X
F -->	FF
X -->	F-[[X]+X]+F[+FX]-X
ø = 22.5
Limit  = 7
base = 0.35

Wavy Seaweed
Axiom =	F
F -->	FF+[+F-F-F]-[-F+F+F]
ø =	22.5
Limit = 5
base = 0.2

Arrow weed
Axiom = X
F --> FF
X --> F[+X][-X]FX
ø =	30
base = 0.35
*/

///L-System variables
string axiom = "F++F++F";
string nxt;
int generation_count = 0;
float len = 2, base = 0.20; //generation 0 values
float angle = 60;
string rule1 = "F-F++F-F"; //F -> rule1
string rule2 = "F-[[X]+X]+F[+FX]-X"; // X -> rule2
float yRotation = 0;

void nextGen(){
    nxt = "";

    for(char c : axiom){
        if(c == 'F'){       ///rule 1
            nxt += rule1;
        }else if(c == 'X'){ ///rule 2
            nxt += rule2;
        }else{
            nxt += c;
        }
    }

    ///decrease branch attributes
    len = len*.67; base -= base*0.3;
    axiom = nxt;
    cout << "generation " << ++generation_count << ": "<< axiom.length() << " in length"<<endl;
    if(generation_count < 3) cout << axiom << endl;
}

///draws branch
void makeCylinder(float height, float base){
    GLUquadric *obj = gluNewQuadric();
    //glColor3f(0.64f, 0.16, 0.16f); //cafe
    glColor3f(0.64f, 1.16, 0.16f); //verde
    //glColor3f(1.0f, 1.0f, 1.0f); //blanco

    glPushMatrix();
        glRotatef(-90, 1.0,0.0,0.0);
        gluCylinder(obj, base,base-(0.2*base), height, 20,20);
    glPopMatrix();

    glutSwapBuffers();
}

void makeTree(float len, float base){ //height = len, base = thickness

    ///start from the bottom of the screen
    glTranslatef(0.0, -5, 0.0);

    for(int i = 0; i < axiom.length(); i++){
        char c = axiom[i];

        ///Turtle
        if(c == 'F'){
            makeCylinder(len, base); glTranslatef(0.0, len, 0.0);
        }else if(c == '+'){
            glRotatef(angle,1,yRotation,1);
        }else if(c == '-'){
            glRotatef(-angle,1,-yRotation,1);
        }else if(c == '['){
            glPushMatrix();
        }else if(c == ']'){
            glColor3f(1.0,0,1.0);
            glutSolidSphere(0.1,10,10); //draws leaf
            glPopMatrix();
        }
    }

}

void init(void){
    ///background color
    glClearColor(0.0,0.0,0.0,0.0);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_DEPTH_TEST);
    makeaTree = glGenLists(1);
    glNewList(makeaTree, GL_COMPILE);
    makeTree(len,base);
    glEndList();
}

void keyboard(unsigned char key, int x, int y){
    switch (key){
        case '+':
            nextGen();
            init();
            glutPostRedisplay();
            break;
        case 27:
            exit(0);
            break;
    }
}

float a = 0.0, delta = 0.1;

void display(){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glPushMatrix();
        glRotatef(x,1.0,0.0,0.0);
        glRotatef(a,0.0,1.0,0.0);
        glRotatef(z,0.0,0.0,1.0);
        glCallList(makeaTree);
    glPopMatrix();
    glutSwapBuffers();

    a += delta;
}

void reshape(int w, int h){
    glViewport (0, 0, (GLsizei) w, (GLsizei) h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(30.0, (GLfloat) w/(GLfloat) h, 0.001, 1000.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0.0,-8.0,-50.0);
}

int main(int argc, char **argv){

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize (1200, 800); glutInitWindowPosition(0,0);
    glutCreateWindow("3D Tree L-System");
    init();
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutDisplayFunc(display);
    glutIdleFunc(display);

    glutMainLoop();
}
