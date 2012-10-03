#include <rho/geo/glhelper.h>

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#include <algorithm>


namespace rho
{
namespace geo
{


void glDraw(tBox b, nRenderMode rm)
{
    // Normalize b so that we can assume things about b.p1 and b.p2.
    b.normalize();

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
    glVertex3d(b.p1.x, b.p1.y, b.p1.z);
    glVertex3d(b.p2.x, b.p1.y, b.p1.z);
    glVertex3d(b.p2.x, b.p2.y, b.p1.z);
    glVertex3d(b.p1.x, b.p2.y, b.p1.z);

    // Left
    glVertex3d(b.p1.x, b.p2.y, b.p2.z);
    glVertex3d(b.p1.x, b.p1.y, b.p2.z);
    glVertex3d(b.p1.x, b.p1.y, b.p1.z);
    glVertex3d(b.p1.x, b.p2.y, b.p1.z);

    // Top
    glVertex3d(b.p2.x, b.p2.y, b.p1.z);
    glVertex3d(b.p2.x, b.p2.y, b.p2.z);
    glVertex3d(b.p1.x, b.p2.y, b.p2.z);
    glVertex3d(b.p1.x, b.p2.y, b.p1.z);

    // Unfortunately we need to pick up the pen here when drawing wireframes.
    if (rm == kWireframe)
    {
        glEnd();
        glBegin(GL_LINE_STRIP);
    }

    // Bottom
    glVertex3d(b.p2.x, b.p1.y, b.p2.z);
    glVertex3d(b.p2.x, b.p1.y, b.p1.z);
    glVertex3d(b.p1.x, b.p1.y, b.p1.z);
    glVertex3d(b.p1.x, b.p1.y, b.p2.z);

    // Back
    glVertex3d(b.p2.x, b.p2.y, b.p2.z);
    glVertex3d(b.p2.x, b.p1.y, b.p2.z);
    glVertex3d(b.p1.x, b.p1.y, b.p2.z);
    glVertex3d(b.p1.x, b.p2.y, b.p2.z);

    // Right
    glVertex3d(b.p2.x, b.p2.y, b.p2.z);
    glVertex3d(b.p2.x, b.p2.y, b.p1.z);
    glVertex3d(b.p2.x, b.p1.y, b.p1.z);
    glVertex3d(b.p2.x, b.p1.y, b.p2.z);

    // End.
    glEnd();
}


}    // namespace geo
}    // namespace rho
