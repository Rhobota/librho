#include <rho/img/tImageCapFactory.h>
#include <rho/tCrashReporter.h>

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#include <iostream>


using namespace rho;
using std::cout;
using std::endl;


refc<img::iImageCap> gImageCap;


void display()
{
    int width  = gImageCap->getParams().imageWidth;
    int height = gImageCap->getParams().imageHeight;

    int bufSize = gImageCap->getRequiredBufSize();
    u8* buffer = new u8[bufSize];

    gImageCap->getFrame(buffer, bufSize);

    glClear(GL_COLOR_BUFFER_BIT);

    glRasterPos2i(-1, 1);
    glPixelZoom(1.0, -1.0);

    glDrawPixels(width, height, GL_RGB, GL_UNSIGNED_BYTE, buffer);

    glutSwapBuffers();

    delete [] buffer;

    glutPostRedisplay();
}


void setupCapture()
{
    img::tImageCapParams params;
    params.deviceURL = "/dev/video0";
    params.captureFormat = img::kYUYV;
    params.displayFormat = img::kRGB24;

    cout << params << endl;

    gImageCap = img::tImageCapFactory::getImageCap(params, true);
}


void setupWindow(int argc, char* argv[])
{
    int width  = gImageCap->getParams().imageWidth;
    int height = gImageCap->getParams().imageHeight;

    glutInit(&argc, argv);

    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(width, height);
    glutCreateWindow("Glut webcam window");

    glutDisplayFunc(display);

    glutMainLoop();
}


int main(int argc, char* argv[])
{
    tCrashReporter::init();

    setupCapture();

    setupWindow(argc, argv);

    cout << "Exiting..." << endl;

    return 0;
}
