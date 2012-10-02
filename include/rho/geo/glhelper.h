#ifndef __rho_geo_glhelper_h__
#define __rho_geo_glhelper_h__


#include <rho/geo/tBox.h>


namespace rho
{
namespace geo
{


enum nRenderMode
{
    kFilled,
    kWireframe
};


void glDraw(const tBox& b, nRenderMode rm = kFilled);


}     // namespace geo
}     // namespace rho


#endif    // __rho_geo_glhelper_h__
