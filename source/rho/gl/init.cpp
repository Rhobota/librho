#ifdef __APPLE__
#include "TargetConditionals.h"
#endif
#if !TARGET_OS_IPHONE && !TARGET_IPHONE_SIMULATOR


#include <rho/gl/init.h>
#include <rho/eRho.h>
#include <rho/geo/units.h>

#include <GL/gl.h>
#include <GL/glu.h>


namespace rho
{
namespace gl
{


void init2d(int width, int height)
{
    /*
     * This makes OpenGL emulate "normal" window coordinates
     * where 0,0 is at the top-left, and positive-y is down.
     */
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glScaled(1.0, -1.0, 1.0);
    gluOrtho2D(0, width, 0, height);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    /*
     * This causes glDrawPixels() to draw downward
     * from the top-left, like a "normal" window,
     * and also like a webcam captures images.
     */
    glPixelZoom(1.0, -1.0);

    /*
     * By default, glDrawPixels() will assume that each row
     * of the image buffer is aligned on 4-byte boundaries.
     * The image buffers in this application are byte-aligned,
     * however. That is, each row follows the previous one
     * immediately.
     */
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    /**
     * Misc
     */
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}


void drawImage2d(int x, int y, rho::img::tImage* image)
{
    glRasterPos2i(x, y);

    switch (image->format())
    {
        case rho::img::kRGB24:
            glDrawPixels(image->width(), image->height(),
                         GL_RGB, GL_UNSIGNED_BYTE, image->buf());
            break;

        default:
            rho::img::tImage alt;
            image->convertToFormat(rho::img::kRGB24, &alt);
            glDrawPixels(alt.width(), alt.height(),
                         GL_RGB, GL_UNSIGNED_BYTE, alt.buf());
            break;
    }
}


void drawCircle2d(int x, int y, int radius, bool filled)
{
    const int kNumLines = 100;

    glPushMatrix();
    glTranslated(x, y, 0.0);

    if (filled)
        glBegin(GL_POLYGON);
    else
        glBegin(GL_LINE_LOOP);

    for (int i = 0; i < kNumLines; i++)
    {
        double x = cos(2.0*rho::geo::kPI*i/kNumLines) * radius;
        double y = sin(2.0*rho::geo::kPI*i/kNumLines) * radius;
        glVertex2d(x, y);
    }

    glEnd();

    glPopMatrix();
}


void init3d(bool cullFront, bool cullBack)
{
    // For 3d applications, all functions/methods should leave
    // the OpenGL state equal to the following before they return.
    // That is, it is okay for a function/method to change some
    // OpenGL state, but they must return it to the following state
    // when they are finished.

    // Enable depth test.
    glEnable(GL_DEPTH_TEST);

    // Have OpenGL normalize all normal-vectors we give it.
    glEnable(GL_NORMALIZE);

    // Enable texturing (textures are created and bound by gl::tArtist)
    glEnable(GL_TEXTURE_2D);

    // Declare that polygons are drawn with the front face's
    // vertices given counter-clockwise (also the default).
    glFrontFace(GL_CCW);

    // Handle culling (discarding-of) polygon faces at fragment
    // rendering time.
    if (cullFront && cullBack)
        glCullFace(GL_FRONT_AND_BACK);
    else if (cullBack)
        glCullFace(GL_BACK);     // <-- this is the default
    else if (cullFront)
        glCullFace(GL_FRONT);
    if (cullFront || cullBack)
        glEnable(GL_CULL_FACE);  // <-- by default not enabled

    // In case the back-facing polygons are not culled, we want to
    // calculate lighting for them properly.
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 1);

    // The specular component should be calculated correctly (instead of
    // the weird -z axis only thing that is done by default).
    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, 1);

    // Enable lighting (use gl::tLight to enable and disable individual lights).
    glEnable(GL_LIGHTING);

    // We will always leave the model-view matrix selected when
    // leaving a function.
    glMatrixMode(GL_MODELVIEW);
}


void orthogonalProjection(int windowWidth, int windowHeight, double width, double depth)
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    double aspectRatio = ((double)windowWidth) / windowHeight;
    glOrtho(-width, width, -width/aspectRatio, width/aspectRatio, 0.0, depth);
    glMatrixMode(GL_MODELVIEW);
}


void perspectiveProjection(
        int windowWidth, int windowHeight,
        double fovy, double near, double far)
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    double aspectRatio = ((double)windowWidth) / windowHeight;
    gluPerspective(fovy, aspectRatio, near, far);
    glMatrixMode(GL_MODELVIEW);
}


}  // namespace gl
}  // namespace rho


#endif
