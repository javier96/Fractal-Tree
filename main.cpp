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
float yRotation = 0;

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
vector<vector<float>> vertices;
vector<vector<int>> faces;
int vertices_count = 0;

void create_vertex(float x, float y, float z) {
  float mv[16];
  glGetFloatv(GL_MODELVIEW_MATRIX, mv);
  for(int i = 0; i < 16; i++) {
      cout << mv[i] << " ";
      if(i%4==0) cout << '\n';
  }
  cout << "-----------------------------\n";
  float xp = mv[0] * x + mv[4] * y + mv[8] * z + mv[12];
  float yp = mv[1] * x + mv[5] * y + mv[9] * z + mv[13];
  float zp = mv[2] * x + mv[6] * y + mv[10] * z + mv[14];
  float wp = mv[3] * x + mv[7] * y + mv[11] * z + mv[15];
  xp /= wp;
  yp /= wp;
  zp /= wp;
   glVertex3f(x, y, z);
  // glVertex3f(xp, yp, zp);
  vertices.push_back({mv[12], mv[13], mv[14]});
}

void make_face() {
  vector<int> face_vertices;
  for(int i = vertices_count; i < vertices.size(); i++) {
    face_vertices.push_back(i);
  }
  faces.push_back(face_vertices);
  vertices_count = vertices.size();
}

void draw_cylinder(GLfloat radius,
                   GLfloat height)
{
    GLfloat x              = 0.0;
    GLfloat y              = 0.0;
    GLfloat angle          = 0.0;
    GLfloat angle_stepsize = 0.1;

    /** Draw the tube */
    glColor3f(0.64f, 1.16, 0.16f);
    glBegin(GL_QUAD_STRIP);
    angle = 0.0;
    bool face = false;
        while( angle < 2*PI ) {
            x = radius * cos(angle);
            y = radius * sin(angle);
            create_vertex(x, y , height);
            create_vertex(x, y , 0.0);
            if(face) {
              make_face();
            }
            angle = angle + angle_stepsize;
            face = !face;
        }
        create_vertex(radius, 0.0, height);
        create_vertex(radius, 0.0, 0.0);
        make_face();
    glEnd();

    /** Draw the circle on top of cylinder */
    glBegin(GL_POLYGON);
    angle = 0.0;
        while( angle < 2*PI ) {
            x = radius * cos(angle);
            y = radius * sin(angle);
            create_vertex(x, y , height);
            angle = angle + angle_stepsize;
        }
        create_vertex(radius, 0.0, height);
        make_face();
    glEnd();
}

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
        draw_cylinder(base, height);
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
        float mv[16];
        glGetFloatv(GL_MODELVIEW_MATRIX, mv);
        for(int i = 0; i < 16; i++) {
            cout << mv[i] << " ";
            if(i%4==0) cout << '\n';
        }
        cout << "-----------------------------\n";
    }

    cout << "---OBJ---" << '\n';
    ofstream file;
    file.open("happy-tree.obj");
    for(vector<float> vertix : vertices) {
      file << "v " << vertix[0] << " " << vertix[1] << " " << vertix[2] << '\n';
    }
    for(vector<int> face : faces) {
      file << "f";
      for(int vertix : face) {
        file <<" " << vertix + 1;
      }
      file << '\n';
    }
    vertices.clear();
    faces.clear();
    vertices_count = 0;
    cout << "---OBJ---\n";
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
