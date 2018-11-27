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
float cam_angle = 0.0f;
float lx = 0.0f;
float lz = -1.0f;
float cx = 0.0f;
float cz = 4.0f;
float rot_a = 0.0f, delta = 0.05;
int slices = 3;

//Defines an L-System
struct System{
    string axiom;
    string nxt;
    int generation_count = 0;
    float len, base, angle;
    int numRules;
    vector<pair<char, string> > rules; //<predecessor, succesor>
    float amp;

    ///Maintain original copies
    string axiom_aux;
    float len_aux, base_aux, angle_aux, amp_aux;
};

System systems[4];
int numSystems;
int curSystem;

// calculates next generation and prepares attributes for new rendering
void nextGen(){
    systems[curSystem].generation_count++;
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
        gluCylinder(obj, base,base-(0.2*base), height, slices, slices);
    glPopMatrix();

    glutSwapBuffers();
}

void makeTree(float len, float base, float angle){
    ///start from the bottom of the screen
    glTranslatef(0.0, -5, 0.0);

    //for every character in string
    for(int i = 0; i < systems[curSystem].axiom.length(); i++){
        char c = systems[curSystem].axiom[i];
        float angle_rand = (float)rand()/RAND_MAX * systems[curSystem].amp;
        if(rand()%2==0){
          angle_rand *= -1;
        }
        ///Turtle graphics
        if(c == 'F'){ // draw and move forward
            makeCylinder(len, base); glTranslatef(0.0, len, 0.0);
        }else if(c == '+'){ //rotate +
            glRotatef(angle_rand, 1, 0, 0);
            glRotatef(angle,1,yRotation,1);
        }else if(c == '-'){ //rotate -
            glRotatef(angle_rand, 1, 0, 0);
            glRotatef(-angle,1,-yRotation,1);
        }else if(c == '['){ //new state
            glPushMatrix();
        }else if(c == ']'){ //pop state
            glColor3f(1.0,0,1.0);
            glutSolidSphere(0.1,10,10); //draws leaf
            glPopMatrix();
        }
    }

}


void init(void){
    cout << "***********params***********";
    cout << "\nMax x angle:\t" << systems[curSystem].amp;
    cout << "\nAngle:\t\t" <<  systems[curSystem].angle;
    cout << "\nResolution:\t" << slices;
    cout << "\nIteration:\t" << systems[curSystem].generation_count;
    cout << "\n--------------------------\n";
   //Lighting settings
   GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
   GLfloat mat_shininess[] = { 80.0 };
   GLfloat light_position[] = { 2.0, 2.0, 2.0, 0.0 };
   glClearColor (0.0, 0.0, 0.0, 0.0);
   glShadeModel (GL_SMOOTH);
   glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
   glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
   glLightfv(GL_LIGHT0, GL_POSITION, light_position);
   glEnable(GL_LIGHTING);
   glEnable(GL_LIGHT0);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_COLOR_MATERIAL);
    makeaTree = glGenLists(1);
    glNewList(makeaTree, GL_COMPILE);
    makeTree(systems[curSystem].len, systems[curSystem].base, systems[curSystem].angle);
    glEndList();
}

// Initiates a new system and resets parameters
void initSystem(int system){
    cout << "Keys:\n1,2,3:\t\tChange tree type.\n+:\t\tRender tree's next iteration\n"
      << "4:\t\tAccess saved session.\n"
      << "a/b:\t\tIncrease/decrease branch angle.\nz/x:\t\tIncrease/decrease x-angle\n"
      << "arrow keys:\tCamera navigation.\ne/r:\t\tRotate tree clockwise & counter-clockwise\n"
      << "n/m:\t\tAdd/remove resolution (slice and stacks)\n"
      << "s:\t\tSave current session\n\n";
    // Reset camera
    lx = 0.0f;
    lz = -1.0f;
    cx = 0.0f;
    cz = 4.0f;
    cam_angle = 0.0f;
    //Reset curSystem's variables
    systems[curSystem].angle = systems[curSystem].angle_aux;
    systems[curSystem].axiom = systems[curSystem].axiom_aux;
    systems[curSystem].len = systems[curSystem].len_aux;
    systems[curSystem].base = systems[curSystem].base_aux;
    systems[curSystem].angle = systems[curSystem].angle_aux;
    systems[curSystem].amp = systems[curSystem].amp_aux;
    systems[curSystem].generation_count = 0;
    curSystem = system;
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
    amp
*/

// Saves the parameters of the current tree being visualized.
void save(){
    systems[3] = systems[curSystem];
    systems[3].axiom_aux = systems[3].axiom;
    systems[3].len_aux = systems[3].len;
    systems[3].base_aux = systems[3].base;
    systems[3].angle_aux = systems[3].angle;
    systems[3].amp_aux = systems[3].amp;

    ofstream cout("L-Systems.txt");

    cout << numSystems << endl << endl;

    System sys;

    for(int i = 0; i < numSystems; i++){
        sys = systems[i];
        cout << sys.numRules << endl;
        for(int j = 0; j < sys.numRules; j++){
            cout << sys.rules[j].first << " " << sys.rules[j].second<<endl;
        }
        cout << sys.axiom_aux << endl;
        cout << sys.len_aux << endl;
        cout << sys.base_aux << endl;
        cout << sys.angle_aux << endl;
        cout << sys.amp_aux << endl<<endl;
    }
}

// keyboard listener for UI
void keyboard(unsigned char key, int x, int y){
    switch (key){
        case '+': //next system generation
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
        case '4':
            initSystem(3); //draw session tree
            init();
            glutPostRedisplay();
            break;
        case 's': //save in session
            save();
            break;
        case 'a': //increment angle for system
            systems[curSystem].angle += 1;
            init();
            glutPostRedisplay();
            break;
        case 'b'://decrement angle for system
            systems[curSystem].angle -= 1;
            init();
            glutPostRedisplay();
            break;
        case 'r': //rotate in +y
            delta = 0.05f;
            break;
        case 'e': //rotate in -y
            delta = -0.05f;
            break;
        case 'n': //add one slice to cylinders
            slices++;
            init();
            glutPostRedisplay();
            break;
        case 'm': //remove one slice from cylinders
            slices--;
            init();
            glutPostRedisplay();
            break;
        case 'z': //add 2 to the max_angle for rotations in x axis
            systems[curSystem].amp += 2.0f;
            init();
            glutPostRedisplay();
            break;
        case 'x': //decrements 2 to the max_angle for rotations in x axis
            systems[curSystem].amp -= 2.0f;
            init();
            glutPostRedisplay();
            break;
        case 27: //exit
            exit(0);
            break;
    }
}

// keyboard listener for UI (Special keys)
void processSpecialKeys(int key, int xx, int yy) {

	float fraction = 1.0f;

	switch (key) {
		case GLUT_KEY_LEFT : // rotate in -y axis
			cam_angle -= 0.05f;
			lx = sin(cam_angle);
			lz = -cos(cam_angle);
			break;
		case GLUT_KEY_RIGHT : // rotate in +y axis
			cam_angle += 0.05f;
			lx = sin(cam_angle);
			lz = -cos(cam_angle);
			break;
		case GLUT_KEY_UP : // move forward
			cx += lx * fraction;
			cz += lz * fraction;
			break;
		case GLUT_KEY_DOWN : // move backward
			cx -= lx * fraction;
			cz -= lz * fraction;
			break;
	}
}

void display(){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glPushMatrix();
      gluLookAt(	cx, 0.0f, cz,
        cx+lx, 0.1f,  cz+lz,
        0.0f, 1.0f,  0.0f);
        glRotatef(rot_a,0.0,1.0,0.0);
        glCallList(makeaTree);
    glPopMatrix();
    glutSwapBuffers();
    rot_a += delta;
}

void reshape(int w, int h){
    glViewport (0, 0, (GLsizei) w, (GLsizei) h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(30.0, (GLfloat) w/(GLfloat) h, 0.001, 1000.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0.0,-4.0,-50.0);
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
    amp
*/

// Loads an L-system from file.
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

        cin >> sys.axiom >> sys.len >> sys.base >> sys.angle >> sys.amp;

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
    glutSpecialFunc(processSpecialKeys);

    glutMainLoop();
}
