#include <stdio.h>
#include <stdlib.h>
#ifdef __APPLE__
#  include <GLUT/glut.h>
#else
#  include <GL/glut.h>
#endif

#define UP 1
#define Down 2
#define LEFT 3
#define RIGHT 4


void drawFloor();
void drawSnake();
void run();
unsigned char camera = 'r';

GLint   lvl      = 1;
int Width = 400;      // window width (pixels)
int Height = 400;     // window height (pixels)

int leftBound = -15;
int rightBound = 15;
int upBound = 15;
int botBound = -15;

int size = 5;
static GLfloat headRotation=90.0F ;

// Snake Variables
GLint   bodyPos[2][100] = {{}};
GLint   _x       = 5;
GLint   _z       = 10;
GLint   _oldX[2] = {};
GLint   _oldZ[2] = {};
GLbyte  direction  = 0;


void keyboardCallback(unsigned char c, int x, int y) {
  switch (c) {
  case 'q':
    exit (0);
    break;

  }
  
  glutPostRedisplay();
}

void reshapeCallback(int w, int h)
{
   Width = w;
   Height = h;
   glViewport(0, 0, w, h);
}

void displayCallback()
{
  // clear the color buffer
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

  // set up camera
  glMatrixMode( GL_PROJECTION );
  glLoadIdentity();
  gluPerspective( 45, 1.0, 0.1, 200.0 );

  glMatrixMode( GL_MODELVIEW );
  glLoadIdentity();

  gluLookAt(0,20,45,0,0,0,0,1,0);
  

  // draw after the opaque objects, since it is translucent
  drawFloor();
  glTranslatef(0,0.5,0);
  //draw snake
  drawSnake();

  // draw the buffer to the screen
  glutPostRedisplay();
  glutSwapBuffers();


  GLenum error = glGetError();
  if(error != GL_NO_ERROR)
    printf("ERROR: %s\n", gluErrorString(error));
}


//---------------------------------------------------------------

void drawFloor() {
  // Draw a floor. Since it is transparent, we need to do it AFTER all of
  // the opaque objects.
  for (int x = -15; x < 16; x++) {
    for (int y = -15; y < 16; y++) {
      glColor4f(1, 1, 1, (x + y) % 2 ? 0.75 : 0.5);
      glNormal3f(0, 1, 0);
      glBegin(GL_POLYGON);
      glVertex3f(x    , 0, y    );
      glVertex3f(x + 1, 0, y    );
      glVertex3f(x + 1, 0, y + 1);
      glVertex3f(x    , 0, y + 1);
      glEnd();
    }
  }
  // visible from initial side angle
  glBegin(GL_POLYGON);
  glVertex3f(-5,    0, 0);
  glVertex3f(-5, -.05, 0);
  glVertex3f( 5, -.05, 0);
  glVertex3f( 5,    0, 0);
  glEnd();
  
  // visible from front angle
  glBegin(GL_POLYGON);
  glVertex3f(0,    0, -5);
  glVertex3f(0, -.05, -5);
  glVertex3f(0, -.05, 5);
  glVertex3f(0,    0, 5);
  glEnd();
}

void drawSnake()
{
    int  i;
    //Drawing the head & the plane
    glPushMatrix();
    //ManipulateViewAngle();

    //Here we will draw the Head of the snake
	glTranslatef(_x,0,_z);//Give it the location according to _x & _z
	glColor3f(1.0,1.0,0.0);
	glutSolidCube(1);

    //Drawing the body
    for(i=0; i<size; i++){//Loop throw the size and draw spheres representing the body
        glPushMatrix();
        //ManipulateViewAngle();
        glTranslatef(bodyPos[0][i],0,bodyPos[1][i]);//this will locate the spheres
        glColor3f(1.0,1.0,0.6);//Color Red
        glutSolidSphere(1,20,20);
        glPopMatrix();
    }
}
//This Function will move the snake according to the directions 
//Taken from the Keyboard keys
void Run(int value){
    int i;

    _oldX[1] = _x;
    _oldZ[1] = _z;
    switch(direction){
      case RIGHT:
          headRotation =90;
          _x += 1;    
          if(_x > rightBound) _x = leftBound;//This will check if the snake is going into the border so it will appear on the other side
          break;
      case LEFT:
          headRotation =-90;
          _x -= 1;    
         if(_x < leftBound) _x = rightBound;//This will check if the snake is going into the border so it will appear on the other side
          break;
      case UP:
          headRotation =0;
          _z += 1;    
          if(_z > upBound) _z = botBound;//This will check if the snake is going into the border so it will appear on the other side
          break;
      case Down:
          headRotation =180;
          _z -= 1;    
          if(_z < botBound) _z = upBound;//This will check if the snake is going into the border so it will appear on the other side
          break;

		glutTimerFunc(130-lvl*4, Run, 0);   
    }

    ////Checks for Collisoin if yes Game Over
    //if(collision()) gameOver = true;

    ////Checks if the snake ate the food (check the X and Y)
    //// If yes it will increase the points & the size of the snake & create a new food
    //if((_x == _bx && _z == _bz) || 
    //((_x >= _bx) && (_x <= _bx + 4) && (_z >= _bz) && (_z <= _bz + 4)) ||
    //((_x <= _bx) && (_x >= _bx - 4) && (_z <= _bz) && (_z >= _bz - 4)) ||
    //((_x <= _bx) && (_x >= _bx - 4) && (_z >= _bz) && (_z <= _bz + 4)) ||
    //((_x >= _bx) && (_x <= _bx + 4) && (_z <= _bz) && (_z >= _bz - 4))){
    //    points++;
    //    if(points < 100) size++;
    //    if(points%5 == 0 && lvl < 15) lvl++;
    //    newFood();
    //}

    for(i = 0; i<size; i++){// Save the positions of the body parts
        _oldX[0]       = _oldX[1];
        _oldZ[0]       = _oldZ[1];
        _oldX[1]       = bodyPos[0][i];
        _oldZ[1]       = bodyPos[1][i];
        bodyPos[0][i] = _oldX[0];
        bodyPos[1][i] = _oldZ[0];
    }

    ////Set the Timer
    glutTimerFunc(130-lvl*4, Run, 0);                    
}


void Special(int key, int x, int y){
    switch(key){
    case GLUT_KEY_RIGHT     :
        if(direction != LEFT)
            direction = RIGHT;
        break;
    case GLUT_KEY_LEFT      :
        if(direction != RIGHT)
            direction = LEFT;
        break;
    case GLUT_KEY_UP        :
        if(direction != UP)
            direction = Down;
        break;
    case GLUT_KEY_DOWN      :
        if(direction != Down)
            direction = UP;
        break;

    }
}
//---------------------------------------------------------------

int main(int argc, char **argv)
{
  // create window and rendering context
  glutInit( &argc, argv );
  glutInitDisplayMode( GLUT_DEPTH | GLUT_RGB | GLUT_DOUBLE );
  glutInitWindowSize( Width, Height );
  glutCreateWindow( "Snake" );
  
  // register display callback
  
  glutSpecialFunc(Special);
  glutKeyboardFunc( keyboardCallback );
  glutDisplayFunc( displayCallback );
  
  glViewport( 0, 0, Width, Height );
  glEnable( GL_DEPTH_TEST );
  glEnable( GL_NORMALIZE );
  
  // Turn on blending (for floor).
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  
  
  // lighting stuff
  GLfloat ambient[] = {0.0, 0.0, 0.0, 1.0};
  GLfloat diffuse[] = {0.9, 0.9, 0.9, 1.0};
  GLfloat specular[] = {0.4, 0.4, 0.4, 1.0};
  GLfloat position0[] = {1.0, 1.0, 1.0, 0.0};
  glLightfv( GL_LIGHT0, GL_POSITION, position0 );
  glLightfv( GL_LIGHT0, GL_AMBIENT, ambient );
  glLightfv( GL_LIGHT0, GL_DIFFUSE, diffuse );
  glLightfv( GL_LIGHT0, GL_SPECULAR, specular );
  GLfloat position1[] = {-1.0, -1.0, -1.0, 0.0};
  glLightfv( GL_LIGHT1, GL_POSITION, position1 );
  glLightfv( GL_LIGHT1, GL_AMBIENT, ambient );
  glLightfv( GL_LIGHT1, GL_DIFFUSE, diffuse );
  glLightfv( GL_LIGHT1, GL_SPECULAR, specular );
  
  glEnable( GL_LIGHTING );
  glEnable( GL_LIGHT0 );
  glEnable( GL_LIGHT1 );
  glEnable( GL_COLOR_MATERIAL );

  
  Run(0);

  // pass control over to GLUT
  glutMainLoop();
  
}

