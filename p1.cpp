#include <stdio.h>
#include <stdlib.h>
#ifdef __APPLE__
#  include <GLUT/glut.h>
#else
#  include <GL/glut.h>
#endif
#include <time.h>
#include <windows.h>

#define UP 1
#define Down 2
#define LEFT 3
#define RIGHT 4

bool collision();
void drawFloor();
void drawSnake();
void drawFood();
void run();
unsigned char camera = 'r';

int Width = 800;      // window width (pixels)
int Height = 800;     // window height (pixels)

// Status Variables
GLint   lvl      = 1;
GLint   points = 0;
GLint   size  = 0;
GLbyte  gameOver = false;
GLbyte  EnableLight = true; 

int leftBound = -15;
int rightBound = 15;
int upBound = 15;
int botBound = -15;

// Variables to check the FPS
DWORD   g_dwLastFPS = 0;
int		g_nFPS = 0, g_nFrames = 0;

// Snake Variables
GLint   bodyPos[2][100] = {{}};
GLint   _x       = 5;
GLint   _z       = 10;
GLint   _oldX[2] = {};
GLint   _oldZ[2] = {};
GLbyte  direction  = 0;

//food pos
GLint _fx = 0;
GLint _fz = 0;

//camera mode

bool moveMode = false;
bool fixMode = true;

void reset()
{
	_x = 5;
	_z = 10;
	direction = 0;
	size = 0;
}

void keyboardCallback(unsigned char c, int x, int y) {
  switch (c) {
  case 'q':
      exit (0);
	  break;
  case 'f':
	  fixMode = true;
	  moveMode = false;
	  break;
  case 'm':
	  moveMode = true;
	  fixMode = false;
  case 'r':
	  gameOver = false;
	  break;

  }
  
  glutPostRedisplay();
}

void Write(char *string){//Write string on the screen
    while(*string)
        glutBitmapCharacter(GLUT_BITMAP_8_BY_13, *string++);
}

void GameStatus(){

    char tmp_str[40];

    // Print the status of the game on the screen
    glColor3f(0, 0, 0);
    glRasterPos2f(0, 13);

    sprintf_s(tmp_str, "Level: %d Points: %d", lvl, points);
    Write(tmp_str);
}

//initialize the first configurations
void Initialize(void) 
{
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.7f, 0.9f, 1.0f, 1.0f); //Change the background to sky blue
    if(EnableLight){
        glEnable(GL_COLOR_MATERIAL); //Enable color
        glEnable(GL_LIGHTING); //Enable lighting
        glEnable(GL_LIGHT0); //Enable light #0
        glEnable(GL_LIGHT1); //Enable light #1
        glEnable(GL_NORMALIZE); //Automatically normalize normals
    }
}

//This Function will calculate the frame per second to display on the screen
void getFPS(){
    char tmp_str[40]; 

    if( GetTickCount() - g_dwLastFPS >= 1000 )				// When A Second Has Passed...
    {
        g_dwLastFPS = GetTickCount();					// Update Our Time Variable
        g_nFPS = g_nFrames;						// Save The FPS
        g_nFrames = 0;							// Reset The FPS Counter
    }
    g_nFrames++;

    glRasterPos2f(0, 14);
    sprintf_s(tmp_str, "FPS: %d", g_nFPS);
    Write(tmp_str);
    
    glRasterPos2f(50, 60);
    sprintf_s(tmp_str, "Pos X: %d Pos Z: %d", _x, _z);
    Write(tmp_str);
}

void reshapeCallback(int w, int h)
{
   Width = w;
   Height = h;
   glViewport(0, 0, w, h);
}

//Display a welcome screen
void WelcomeScreen(){
    char tmp_str[40];

    glColor3f(1, 0, 0);
    glRasterPos2f(0, 2);
    Write("Welcome To Snake 3D Game.");
    
    glColor3f(1, 0, 0);
    glRasterPos2f(0, 0);
    Write("To Start Playing please press 'r'. Enjoy");
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


  if(fixMode)
  gluLookAt(0,30,40,0,0,0,0,1,0);

  if(moveMode)
  {

    switch(direction){
      case RIGHT:
	glTranslatef(0,0,-30);
    gluLookAt(-3,3,0,0,0,0,0,1,0);	 
	glTranslatef(-_x,0,-_z);            
          break;
      case LEFT:
	glTranslatef(0,0,-30);
    gluLookAt(3,3,0,0,0,0,0,1,0);	 
	glTranslatef(-_x,0,-_z);  
          break;
      case UP:
	glTranslatef(0,0,-30);
    gluLookAt(0,3,-3,0,0,0,0,1,0);	 
	glTranslatef(-_x,0,-_z);          
          break;
      case Down:
	glTranslatef(0,0,-30);
    gluLookAt(0,3,3,0,0,0,0,1,0);	 
	glTranslatef(-_x,0,-_z);           
          break;
	  default:
	glTranslatef(0,0,-30);
    gluLookAt(0,3,3,0,0,0,0,1,0);	 
	glTranslatef(-_x,0,-_z); 
	}

  }
  
  if(!gameOver)
  {

	// draw after the opaque objects, since it is translucent
	drawFloor();
	glTranslatef(0,0.5,0);
	//draw snake
	drawSnake();
	drawFood();
  }
  else{
	  WelcomeScreen();
	  reset();
  }

      getFPS();
  GameStatus();
  // draw the buffer to the screen
  glutPostRedisplay();
  glutSwapBuffers();


  GLenum error = glGetError();
  //if(error != GL_NO_ERROR)
    //printf("ERROR: %s\n", gluErrorString(error));
}


//Generate the New Food that the snake will eat
void newFood(){
    time_t seconds;
    time(&seconds);
    srand((unsigned int) seconds);
    _fx = rand() % 30 + leftBound; 
    _fz = rand() % 30 + botBound;
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

	glPopMatrix();
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

void drawFood()
{
    //Draw the Sphere representing the Food for the snake
    glPushMatrix();
    glTranslatef(_fx,0,_fz);
    glColor3f(0.8, 0.4, 0.4);
    glutSolidSphere(1,20,20);
    glPopMatrix();
}
//This Function will move the snake according to the directions 
//Taken from the Keyboard keys
void Run(int value){
    int i;

    _oldX[1] = _x;
    _oldZ[1] = _z;


    switch(direction){
      case RIGHT:
          _x += 1;    
          if(_x > rightBound) _x = leftBound;//This will check if the snake is going into the border so it will appear on the other side
          break;
      case LEFT:
          _x -= 1;    
         if(_x < leftBound) _x = rightBound;//This will check if the snake is going into the border so it will appear on the other side
          break;
      case UP:
          _z += 1;    
          if(_z > upBound) _z = botBound;//This will check if the snake is going into the border so it will appear on the other side
          break;
      case Down:
          _z -= 1;    
          if(_z < botBound) _z = upBound;//This will check if the snake is going into the border so it will appear on the other side
          break;
		}

    //Checks for Collisoin if yes Game Over
    if(collision()) gameOver = true;

    //Checks if the snake ate the food (check the X and Y)
    // If yes it will increase the points & the size of the snake & create a new food
    if((_x == _fx && _z == _fz) || 
    ((_x >= _fx) && (_x <= _fx + 1) && (_z >= _fz) && (_z <= _fz + 1)) ||
    ((_x <= _fx) && (_x >= _fx - 1) && (_z <= _fz) && (_z >= _fz - 1)) ||
    ((_x <= _fx) && (_x >= _fx - 1) && (_z >= _fz) && (_z <= _fz + 1)) ||
    ((_x >= _fx) && (_x <= _fx + 1) && (_z <= _fz) && (_z >= _fz - 1))){
        points++;
        if(points < 100) size++;
        if(points%5 == 0 && lvl < 15) lvl++;
        newFood();
    }

    for(i = 0; i<size; i++){// Save the positions of the body parts
        _oldX[0]       = _oldX[1];
        _oldZ[0]       = _oldZ[1];
        _oldX[1]       = bodyPos[0][i];
        _oldZ[1]       = bodyPos[1][i];
        bodyPos[0][i] = _oldX[0];
        bodyPos[1][i] = _oldZ[0];
    }


    //Set the Timer
    glutTimerFunc(130-lvl*4, Run, 0);
	//glutTimerFunc(30, Run, 0); 
	//glutPostRedisplay();
}

//This Function Will Check for Collision
bool collision(){
    int i;

    for(i=0; i<size; i++){
        if(bodyPos[0][i] == _x && bodyPos[1][i] == _z) 
        return true;
    }
    return false;
}

void Special(int key, int x, int y){
	if(fixMode)
	{
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
	else
	{
		switch(key){
		case GLUT_KEY_RIGHT     :
        if(direction == LEFT)
            direction = Down;
		else if(direction == RIGHT)
			direction = UP;
		else if(direction == UP)
			direction = LEFT;
		else if(direction == Down)
			direction = RIGHT;
        break;

		case GLUT_KEY_LEFT      :
        if(direction == LEFT)
            direction = UP;
		else if(direction == RIGHT)
			direction = Down;
		else if(direction == UP)
			direction = RIGHT;
		else if(direction == Down)
			direction = LEFT;
        break;
		}
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
  Initialize();
  // pass control over to GLUT
  glutMainLoop();
  
}

