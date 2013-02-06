#include <rho/gl/init.h>
#include <rho/eRho.h>

#include <GL/gl.h>
#include <GL/glu.h>


namespace rho
{
namespace gl
{


void init2d(int width, int height)
{
    throw eNotImplemented("not implemented!");
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
