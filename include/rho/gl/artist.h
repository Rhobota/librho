#ifndef __rho_gl_glhelper_h__
#define __rho_gl_glhelper_h__


#include <rho/geo/tBox.h>
#include <rho/geo/tMesh.h>
#include <rho/geo/tRect.h>


namespace rho
{
namespace gl
{


enum nRenderMode
{
    kFilled,
    kWireframe
};


void draw(geo::tBox b, nRenderMode rm = kFilled);

void draw(geo::tRect r, nRenderMode rm = kFilled);

void draw(const geo::tMesh& m, nRenderMode rm = kFilled);


}     // namespace gl
}     // namespace rho


#endif    // __rho_gl_glhelper_h__
