#include <rho/img/tImage.h>
#include <rho/tCrashReporter.h>

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#include <cmath>
#include <iostream>


using namespace rho;
using std::cout;
using std::endl;
using std::cin;


img::tImage gImage("eyes/eye_example_leuk.png", img::kRGB24);


void display()
{
    glRasterPos2i(-1, 1);
    glPixelZoom(1.0, -1.0);

    glClear(GL_COLOR_BUFFER_BIT);
    glDrawPixels(gImage.width(), gImage.height(),
                 GL_RGB, GL_UNSIGNED_BYTE, gImage.buf());

    glutSwapBuffers();
}


void setupWindow(int argc, char* argv[])
{
    int width  = gImage.width();
    int height = gImage.height();

    glutInit(&argc, argv);

    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(width, height);
    glutCreateWindow("Glut image window");

    glutDisplayFunc(display);

    glutMainLoop();
}


int main(int argc, char* argv[])
{
    tCrashReporter::init();

    setupWindow(argc, argv);

    return 0;
}
