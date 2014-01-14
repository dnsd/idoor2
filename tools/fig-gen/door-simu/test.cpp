#include <iostream>
#include <GL/gl.h>
#include <GL/glut.h>

using namespace std;

void disp( void ) {
	glClear(GL_COLOR_BUFFER_BIT);
}

void key(unsigned char key , int x , int y) {
	// printf("Key = %c\n" , key);
	cout << "key = " << key << endl;
}

int main(int argc , char ** argv) {
	glutInit(&argc , argv);
	glutInitWindowPosition(100 , 50);
	glutInitWindowSize(400 , 300);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);

	glutCreateWindow("Kitty on your lap");
	glutDisplayFunc(disp);
	glutKeyboardFunc(key);

	glutMainLoop();
	return 0;
}