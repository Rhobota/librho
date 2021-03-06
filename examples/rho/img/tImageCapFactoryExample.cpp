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

img::tImage gImage;
img::tImage gImageEdges;
u32 gClipAtValue = 255;


void display()
{
    gImageCap->getFrame(&gImage);
    gImage.verticalFlip();
    gImage.sobel(&gImageEdges, gClipAtValue);

    glRasterPos2i(-1, 1);
    glPixelZoom(1.0, -1.0);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glClear(GL_COLOR_BUFFER_BIT);
    glDrawPixels(gImageEdges.width(), gImageEdges.height(),
                 GL_RGB, GL_UNSIGNED_BYTE, gImageEdges.buf());

    glutSwapBuffers();
    glutPostRedisplay();
}


void keyboard(unsigned char key, int x, int y)
{
    if (key == 'w')
        gClipAtValue += 10;
    if (key == 's')
        gClipAtValue -= 10;
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

    gImageCap = img::tImageCapFactory::getImageCap(params, false);
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
    glutKeyboardFunc(keyboard);

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
