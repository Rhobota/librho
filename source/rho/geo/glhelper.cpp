#include <rho/geo/glhelper.h>

#include <GL/gl.h>
#include <GL/glu.h>

#include <algorithm>


namespace rho
{
namespace geo
{


void glDraw(tBox b, nRenderMode rm)
{
    // Normalize b so that we can assume things about b.p1 and b.p2.
    b = b.normalize();

    // Begin.
    if (rm == kFilled)
        glBegin(GL_QUADS);
    else if (rm == kWireframe)
        glBegin(GL_LINE_STRIP);
    else
        throw eLogicError("Invalid nRenderMode.");

    // The following comments assumes you're looking at the cube down
    // the negative z-axis. Positive x is to the right. Positive y is up.

    // Front
    glNormal3d(0.0, 0.0, 1.0);
    glVertex3d(b.p2.x, b.p2.y, b.p2.z);
    glVertex3d(b.p1.x, b.p2.y, b.p2.z);
    glVertex3d(b.p1.x, b.p1.y, b.p2.z);
    glVertex3d(b.p2.x, b.p1.y, b.p2.z);

    // Right
    glNormal3d(1.0, 0.0, 0.0);
    glVertex3d(b.p2.x, b.p2.y, b.p2.z);
    glVertex3d(b.p2.x, b.p1.y, b.p2.z);
    glVertex3d(b.p2.x, b.p1.y, b.p1.z);
    glVertex3d(b.p2.x, b.p2.y, b.p1.z);

    // Top
    glNormal3d(0.0, 1.0, 0.0);
    glVertex3d(b.p2.x, b.p2.y, b.p2.z);
    glVertex3d(b.p2.x, b.p2.y, b.p1.z);
    glVertex3d(b.p1.x, b.p2.y, b.p1.z);
    glVertex3d(b.p1.x, b.p2.y, b.p2.z);

    // Unfortunately, we need to lift the pen here for wireframe drawing.
    if (rm == kWireframe)
    {
        glVertex3d(b.p2.x, b.p2.y, b.p2.z);
        glEnd();
        glBegin(GL_LINE_STRIP);
    }

    // Back
    glNormal3d(0.0, 0.0, -1.0);
    glVertex3d(b.p1.x, b.p1.y, b.p1.z);
    glVertex3d(b.p1.x, b.p2.y, b.p1.z);
    glVertex3d(b.p2.x, b.p2.y, b.p1.z);
    glVertex3d(b.p2.x, b.p1.y, b.p1.z);

    // Left
    glNormal3d(-1.0, 0.0, 0.0);
    glVertex3d(b.p1.x, b.p1.y, b.p1.z);
    glVertex3d(b.p1.x, b.p1.y, b.p2.z);
    glVertex3d(b.p1.x, b.p2.y, b.p2.z);
    glVertex3d(b.p1.x, b.p2.y, b.p1.z);

    // Bottom
    glNormal3d(0.0, -1.0, 0.0);
    glVertex3d(b.p1.x, b.p1.y, b.p1.z);
    glVertex3d(b.p2.x, b.p1.y, b.p1.z);
    glVertex3d(b.p2.x, b.p1.y, b.p2.z);
    glVertex3d(b.p1.x, b.p1.y, b.p2.z);

    // Another annoying wireframe thing.
    if (rm == kWireframe)
    {
        glVertex3d(b.p1.x, b.p1.y, b.p1.z);
    }

    // End.
    glEnd();
}


void glDraw(tRect r, nRenderMode rm)
{
    // Begin.
    if (rm == kFilled)
        glBegin(GL_QUADS);
    else if (rm == kWireframe)
        glBegin(GL_LINE_STRIP);
    else
        throw eLogicError("Invalid nRenderMode.");

    // One one face...
    glVertex2d(r.x, r.y);
    glVertex2d(r.x+r.width, r.y);
    glVertex2d(r.x+r.width, r.y+r.height);
    glVertex2d(r.x, r.y+r.height);
    if (rm == kWireframe)
        glVertex2d(r.x, r.y);

    // End.
    glEnd();
}


}    // namespace geo
}    // namespace rho
