#include <rho/geo/glhelper.h>

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>


namespace rho
{
namespace geo
{


void glDraw(const tRect& r)
{
    glBegin(GL_QUADS);
    glVertex2i(r.x, r.y);
    glVertex2i(r.x, r.y+r.height);
    glVertex2i(r.x+r.width, r.y+r.height);
    glVertex2i(r.x+r.width, r.y);
    glEnd();
}


}    // namespace geo
}    // namespace rho
