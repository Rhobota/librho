#include <rho/geo/glhelper.h>

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>


namespace rho
{
namespace geo
{


void glDraw(const tBox& b, nRenderMode rm)
{
    if (rm == kFilled)
        glBegin(GL_QUADS);
    else if (rm == kWireframe)
        glBegin(GL_LINE_LOOP);
    glVertex2i(r.x, r.y);
    glVertex2i(r.x, r.y+r.height);
    glVertex2i(r.x+r.width, r.y+r.height);
    glVertex2i(r.x+r.width, r.y);
    glEnd();
}


}    // namespace geo
}    // namespace rho
