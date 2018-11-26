#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <math.h>
#include <time.h>
#include <iostream>
#include <vector>
#include <string.h>
#include <fstream>
using namespace std;
#include <GL/glut.h>
const double PI = acos(-1);

#define debug(s) cout<< #s <<" = "<< s <<endl

GLuint makeaTree;
float x,y,z;
float yRotation = 0.1; //0 para 2D, lo que sea para 3D

struct System{
    string axiom;
    string nxt;
    int generation_count = 0;
    float len, base, angle;
    int limit; //limited number of generations
    int numRules;
    vector<pair<char, string> > rules; //<predecessor, succesor>
    float translateDown;

    ///Maintain original copies
    string axiom_aux;
    float len_aux, base_aux, angle_aux;
};

System systems[4];
int numSystems;
int curSystem;

void nextGen(){
    systems[curSystem].nxt = "";

    for(char c : systems[curSystem].axiom){
        bool found = false;

        for(int i = 0; i < systems[curSystem].numRules; i++){
            if(c == systems[curSystem].rules[i].first){
                found = true;
                systems[curSystem].nxt += systems[curSystem].rules[i].second;
            }
        }

        if(!found){
            systems[curSystem].nxt += c;
        }
    }

    ///decrease branch attributes
    systems[curSystem].len = systems[curSystem].len*.67;
    systems[curSystem].base -= systems[curSystem].base*0.3;
    systems[curSystem].axiom = systems[curSystem].nxt;

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

void makeTree(float len, float base, float angle){

    ///start from the bottom of the screen
    glTranslatef(0.0, systems[curSystem].translateDown, 0.0);

    for(int i = 0; i < systems[curSystem].axiom.length(); i++){
        char c = systems[curSystem].axiom[i];

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
    makeTree(systems[curSystem].len, systems[curSystem].base, systems[curSystem].angle);
    glEndList();
}

void initSystem(int system){
    curSystem = system;
    systems[curSystem].axiom = systems[curSystem].axiom_aux;
    systems[curSystem].len = systems[curSystem].len_aux;
    systems[curSystem].base = systems[curSystem].base_aux;
    systems[curSystem].angle = systems[curSystem].angle_aux;
    systems[curSystem].generation_count = 0;
}

void keyboard(unsigned char key, int x, int y){
    switch (key){
        case '+':
            nextGen();
            init();
            glutPostRedisplay();
            break;
        case '1':
            initSystem(0); //draw fuzzy weed
            init();
            glutPostRedisplay();
            break;
        case '2':
            initSystem(1); //draw wavy seaweed
            init();
            glutPostRedisplay();
            break;
        case '3':
            initSystem(2); //draw arrow weed
            init();
            glutPostRedisplay();
            break;
        case 'a':
            systems[curSystem].angle += 1;
            init();
            glutPostRedisplay();
            break;
        case 'b':
            systems[curSystem].angle -= 1;
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


/*
Formato de input

numSystems

<numSystems>
    numRules
    <numRules>
        predecessor succesor
    axiom
    len
    base
    angle
    limit
    translateDown
*/
void loadLSystems(){
    ifstream cin("L-Systems.txt");

    cin >> numSystems;

    char predecessor;
    string successor;

    for(int i = 0; i < numSystems; i++){
        System sys;
        cin >> sys.numRules;

        for(int j = 0; j < sys.numRules; j++){
            cin >> predecessor >> successor;
            sys.rules.push_back({predecessor, successor});
        }

        cin >> sys.axiom >> sys.len >> sys.base >> sys.angle >> sys.limit >> sys.translateDown;

        sys.axiom_aux = sys.axiom;
        sys.len_aux = sys.len;
        sys.base_aux = sys.base;
        sys.angle_aux = sys.angle;

        systems[i] = sys;
    }

    ///init a system
    initSystem(0);

}

int main(int argc, char **argv){

    //load L-Systems
    loadLSystems();

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize (1000, 1000); glutInitWindowPosition(0,0);
    glutCreateWindow("3D Tree L-System");
    init();
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutDisplayFunc(display);
    glutIdleFunc(display);

    glutMainLoop();
}
