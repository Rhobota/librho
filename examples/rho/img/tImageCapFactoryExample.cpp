#include <rho/img/tImageCapFactory.h>
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


refc<img::iImageCap> gImageCap;


void verticalFlip(u8* buf, int width, int height)
{
    for (int h = 0; h < height; h++)
    {
        u8* row = buf + h*width*3;
        for (int l=0, r=width-1; l < width/2; l++, r--)
        {
            u8* lp = row + l*3;
            u8* rp = row + r*3;
            std::swap(lp[0], rp[0]);
            std::swap(lp[1], rp[1]);
            std::swap(lp[2], rp[2]);
        }
    }
}


void display()
{
    int width  = gImageCap->getParams().imageWidth;
    int height = gImageCap->getParams().imageHeight;

    int bufSize = gImageCap->getRequiredBufSize();
    static u8* buffer = NULL;
    if (!buffer)
        buffer = new u8[bufSize];

    //int readSize =
    gImageCap->getFrame(buffer, bufSize);

    verticalFlip(buffer, width, height);

    glClear(GL_COLOR_BUFFER_BIT);

    glRasterPos2i(-1, 1);
    glPixelZoom(1.0, -1.0);

    glDrawPixels(width, height, GL_RGB, GL_UNSIGNED_BYTE, buffer);

    glutSwapBuffers();

    glutPostRedisplay();
}


void setupCapture()
{
    refc<img::iImageCapParamsEnumerator> enumerator =
        img::tImageCapFactory::getImageCapParamsEnumerator();

    for (int i = 0; i < enumerator->size(); i++)
    {
        cout << "----- OPTION #" << i << " -----" << endl;
        cout << (*enumerator)[i] << endl;
    }

    cout << "Which option? ";
    int op;
    cin >> op;

    img::tImageCapParams params = (*enumerator)[op];
    params.displayFormat = img::kRGB24;

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
