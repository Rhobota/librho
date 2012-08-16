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


void verticalFlip(img::tImage* image)
{
    u8* buf     = image->buf();
    u32 bufUsed = image->bufUsed();
    u32 width   = image->width();
    u32 height  = image->height();

    u32 bpp = bufUsed / (width * height);   // bytes-per-pixel

    for (u32 h = 0; h < height; h++)
    {
        u8* row = buf + (h * width * bpp);
        for (u32 l=0, r=width-1; l < width/2; l++, r--)
        {
            u8* lp = row + (l * bpp);
            u8* rp = row + (r * bpp);
            for (u32 i = 0; i < bpp; i++)
                std::swap(lp[i], rp[i]);
        }
    }
}


void display()
{
    gImageCap->getFrame(&gImage);
    verticalFlip(&gImage);

    glRasterPos2i(-1, 1);
    glPixelZoom(1.0, -1.0);

    glClear(GL_COLOR_BUFFER_BIT);
    glDrawPixels(gImage.width(), gImage.height(),
                 GL_RGB, GL_UNSIGNED_BYTE, gImage.buf());

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

    gImageCap = img::tImageCapFactory::getImageCap(params, false);

    gImage.setBufSize( gImageCap->getRequiredBufSize() );
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
