#include <stdlib.h>
#include <iostream>
#include <fstream>
using namespace std;
#include <glut.h>

bool mouseLeftDown;
bool mouseRightDown;
float mouseX, mouseY;
float cameraDistance;
float cameraAngleX;
float cameraAngleY;

char* datapath;
// array stored vertices and faces.
GLfloat vArr[512][3];
GLint nArr[1000][3];

// array stored colors defined by RGB.
GLfloat mat[4];

// whether to rotate.
bool IS_ROTATE = false;
// whether to change color.
bool IS_CHANGE_COLOR = false;

// the indicator of color used to change the color.
unsigned int indicator_corlor = 0;
// the basic parameters x and y of light location.
GLfloat lightfrom_x = 1.0, lightfrom_y = 1.0;

// set color
void set_pen_color() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	mat[0] = 1 - 0.1*indicator_corlor; mat[1] = 1; mat[2] = 1;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat);
}
/*
 * Initialize depth buffer, projection matrix, light source, and lighting
 * model.  Do not specify a material property here.
 */
void init(void)
{
	GLfloat ambient[] = { 0.0, 0.0, 0.0, 1.0 };
	GLfloat diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat specular[] = { 1.0, 1.0, 1.0, 1.0 };

	//光源位置
	GLfloat position[] = { lightfrom_x, lightfrom_y, 5.0, 0.0 };

	GLfloat lmodel_ambient[] = { 0.2, 0.2, 0.2, 1.0 };
	GLfloat local_view[] = { 0.0 };

	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT0, GL_POSITION, position);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
	glLightModelfv(GL_LIGHT_MODEL_LOCAL_VIEWER, local_view);

	glFrontFace(GL_CW);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_AUTO_NORMAL);
	glEnable(GL_NORMALIZE);
	glEnable(GL_DEPTH_TEST);
}
// draw shape by data file.
void draw_shape() {
	glTranslatef(0, 0, cameraDistance);
	glRotatef(cameraAngleX, 1, 0, 0);
	glRotatef(cameraAngleY, 0, 1, 0);

	fstream f(datapath, ios::in);
	char k;
	GLfloat* vp = vArr[0];
	GLint* np = nArr[0];
	int vnum = 0, nnum = 0;
	while (f >> k)
	{
		if (k == 'v') {
			for (int i = 0; i < 3; i++) {
				f >> *vp;
				vp++;
			}
			vnum++;
		}
		else if (k == 'f') {
			for (int i = 0; i < 3; i++) {
				f >> *np;
				np++;
			}
			nnum++;
		}
	}
	f.close();
	vp = vArr[0];
	np = nArr[0];

	set_pen_color();
	int n = 1;
	if (abs(vArr[nArr[0][0] - 1][0]) < 0.2) n = 40;
	else n = 3;
	for (int i = 0; i < nnum; i++)
	{
		//glBegin(GL_TRIANGLES);
		//glVertex3d(vArr[nArr[i][0] - 1][0]*20, vArr[nArr[i][0] - 1][1] * 20, vArr[nArr[i][0] - 1][2] * 20);
		//glVertex3d(vArr[nArr[i][1] - 1][0] * 20, vArr[nArr[i][1] - 1][1] * 20, vArr[nArr[i][1] - 1][2] * 20);
		//glVertex3d(vArr[nArr[i][2] - 1][0] * 20, vArr[nArr[i][2] - 1][1] * 20, vArr[nArr[i][2] - 1][2] * 20);
		//glEnd();
		glBegin(GL_LINES);
		glVertex3d(vArr[nArr[i][0] - 1][0] * n, vArr[nArr[i][0] - 1][1] * n, vArr[nArr[i][0] - 1][2] * n);
		glVertex3d(vArr[nArr[i][1] - 1][0] * n, vArr[nArr[i][1] - 1][1] * n, vArr[nArr[i][1] - 1][2] * n);
		glVertex3d(vArr[nArr[i][0] - 1][0] * n, vArr[nArr[i][0] - 1][1] * n, vArr[nArr[i][0] - 1][2] * n);
		glVertex3d(vArr[nArr[i][2] - 1][0] * n, vArr[nArr[i][2] - 1][1] * n, vArr[nArr[i][2] - 1][2] * n);
		glVertex3d(vArr[nArr[i][1] - 1][0] * n, vArr[nArr[i][1] - 1][1] * n, vArr[nArr[i][1] - 1][2] * n);
		glVertex3d(vArr[nArr[i][2] - 1][0] * n, vArr[nArr[i][2] - 1][1] * n, vArr[nArr[i][2] - 1][2] * n);
		glEnd();
	}
}
void display(void)
{
	GLfloat mat[4];
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPushMatrix();
	mat[0] = 1; mat[1] = 1; mat[2] = 1;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat);
	draw_shape();
	glPopMatrix();
	glFlush();
}

void reshape(int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if (w <= h)
		glOrtho(-16.0, 16.0, -16.0, 16.0*(GLfloat)h / (GLfloat)w,
			-10.0, 10.0);
	else
		glOrtho(-16.0, 16.0*(GLfloat)w / (GLfloat)h, -16.0, 16.0,
			-10.0, 10.0);
	glMatrixMode(GL_MODELVIEW);
}

// set color and draw shape.
void drawScene() {
	set_pen_color();
	draw_shape();
	glFlush();
}
void keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 27:
		exit(0);
		break;
	case 'c':
		IS_CHANGE_COLOR = !IS_CHANGE_COLOR;
		break;
	case 'r':
		IS_ROTATE = !IS_ROTATE;
		break;
	case '-':
		glScalef(0.9, 0.9, 0.9);
		glutPostRedisplay();
		break;
	case '+':
		glScalef(1 / 0.9, 1 / 0.9, 1 / 0.9);
		glutPostRedisplay();
		break;
	default:
		cout << "Unhandled key press " << key << "." << endl;
		break;
	}
}
void special_key(GLint key, GLint x, GLint y) {
	switch (key) {
	case GLUT_KEY_UP:
		lightfrom_y += 0.5;
		break;
	case GLUT_KEY_DOWN:
		lightfrom_y -= 0.5;
		break;
	case GLUT_KEY_LEFT:
		lightfrom_x -= 0.5;
		break;
	case GLUT_KEY_RIGHT:
		lightfrom_x += 0.5;
		break;
	default:
		break;
	}
	GLfloat position[] = { lightfrom_x, lightfrom_y, 5.0, 0.0 };
	glLightfv(GL_LIGHT0, GL_POSITION, position);
	glutPostRedisplay();
}

// time function for rotating OXY with y-axis.
void time_rotate(int value) {
	if (value == 1) {
		if (IS_ROTATE) {
			glRotatef(5, 0, 1, 0);
			glutPostRedisplay();
		}
		glutTimerFunc(100, time_rotate, 1);
	}
}

// time function for change color.
void time_change_color(int value) {
	if (value == 2) {
		if (IS_CHANGE_COLOR) {
			indicator_corlor = (indicator_corlor + 1) % 9;
			drawScene();
		}
		glutTimerFunc(100, time_change_color, 2);
	}
}

void mouseCB(int button, int state, int x, int y)
{
	mouseX = x;
	mouseY = y;

	if (button == GLUT_LEFT_BUTTON)
	{
		if (state == GLUT_DOWN)
		{
			mouseLeftDown = true;
		}
		else if (state == GLUT_UP)
			mouseLeftDown = false;
	}
	else if (button == GLUT_RIGHT_BUTTON)
	{
		if (state == GLUT_DOWN)
		{
			mouseRightDown = true;
		}
		else if (state == GLUT_UP)
			mouseRightDown = false;
	}
}

void mouseMotionCB(int x, int y)
{
	if (mouseLeftDown)
	{
		cameraAngleY += (x - mouseX);
		cameraAngleX += (y - mouseY);
		mouseX = x;
		mouseY = y;
	}
	if (mouseRightDown)
	{
		cameraDistance += (y - mouseY) * 0.2f;
		mouseY = y;
	}
	glutPostRedisplay();
}
/*
 * Main Loop
 */
int main(int argc, char **argv)
{
	if (argc < 2) {
		cout << "valid input! lack of parameters!";
		exit(0);
	}
	datapath = argv[1];
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(400, 400);
	glutInitWindowPosition(150, 150);
	glutCreateWindow("Work No.1");
	init();
	glutReshapeFunc(reshape);
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(special_key);
	glutTimerFunc(100, time_rotate, 1);
	glutTimerFunc(100, time_change_color, 2);
	glutMouseFunc(mouseCB);
	glutMotionFunc(mouseMotionCB);
	glutMainLoop();
	return 0;
}
