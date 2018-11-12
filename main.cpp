#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <math.h>
#include <time.h>
#include <iostream>
using namespace std;
#include <GL/glut.h>
const double PI = acos(-1);

GLuint makeaTree;
float x,y,z;

/*
Axiom	X
F -->	FF
X -->	F-[[X]+X]+F[+FX]-X
ø = 22.5

Axiom	F+F+F+F
F -->	FF+F-F+F+FF
ø =	90

Axiom	F
F -->	FF+[+F-F-F]-[-F+F+F]
ø =	22.5

*/

///variables de L-System
string axiom = "F", nxt;
int generation_count = 0;
float len = 2, base = 0.2;
float angle = 22.5;
string rule = "FF+[+F-F-F]-[-F+F+F]";
string rule2 = "F-[[X]+X]+F[+FX]-X";

void makeCylinder(float height, float base){
    GLUquadric *obj = gluNewQuadric();
    //gluQuadricDrawStyle(obj, GLU_LINE);
    glColor3f(0.64f, 0.16, 0.16f);
    glPushMatrix();
        glRotatef(-90, 1.0,0.0,0.0);
        gluCylinder(obj, base,base-(0.2*base), height, 20,20);
    glPopMatrix();
    glutSwapBuffers();
}

void makeTree(float len, float base){ //height = len, base = thickness

    //float angle;
    //makeCylinder(height, base); glTranslatef(0.0, height, 0.0);
    //height -= height*.2; base-= base*0.3;
    /*
    for(int a= 0; a<3; a++){
        angle = rand()%50+20;
        if(angle >48){
            angle = -(rand()%50+20);
        }

        if (height > 1){
            glPushMatrix();
                glRotatef(angle,1,0.0,1);
                makeTree(height,base);
            glPopMatrix();
        }else {
            glColor3f(0.0,1.0/a,0.0);
            glutSolidSphere(.2,10,10);
        }
    }
    */

    ///que empiece desde abajo
    glTranslatef(0.0, -5, 0.0);

    for(int i = 0; i < axiom.length(); i++){
        char c = axiom[i];

        if(c == 'F'){

            ///hojitas deben aparecer bajo una condicion
            //glColor3f(0.0,1.0,0.0);
            //glutSolidSphere(base,10,10);

            makeCylinder(len, base); glTranslatef(0.0, len, 0.0);

        }else if(c == '+'){
            glRotatef(angle,1,0.0,1);
        }else if(c == '-'){
            glRotatef(-angle,1,0.0,1);
        }else if(c == '['){
            glPushMatrix();
        }else if(c == ']'){
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

void nextGen(){
    nxt = "";
    for(int i = 0; i < axiom.length(); i++){
        char c = axiom[i];
        if(c == 'F'){
            nxt += rule;
        }else if(c == 'X'){
            nxt += rule2;
        }
        else{
            nxt += c;
        }
    }

    ///por definir bien
    len = len*.67; base -= base*0.3;
    axiom = nxt;
    cout << "generation " << generation_count << ": "<<nxt << endl;
    generation_count++;
}

void keyboard(unsigned char key, int x, int y){
    switch (key){
        case 'x':
            x +=10;
            glutPostRedisplay();
            break;
        case 'y':
            y +=10;
            glutPostRedisplay();
            break;
        case 'z':
            z+=10;
            glutPostRedisplay();
            break;
        case '+':
            nextGen();
            init();
            glutPostRedisplay();
            break;
    }
}

void display(){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glPushMatrix();
        glRotatef(x,1.0,0.0,0.0);
        glRotatef(y,0.0,1.0,0.0);
        glRotatef(z,0.0,0.0,1.0);
        glCallList(makeaTree);
    glPopMatrix();
    glutSwapBuffers();
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
    glutMainLoop();

}
