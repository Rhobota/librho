#ifndef __rho_geo_tLine_h__
#define __rho_geo_tLine_h__


#include <rho/geo/tPoint.h>
#include <rho/geo/tRect.h>


namespace rho
{
namespace geo
{


class tLine
{
    public:

        tLine();

        tRect rect();

    public:

        tPoint p1, p2;
};


}   // namespace geo
}   // namespace rho


#endif    // __rho_geo_tLine_h__
