#ifndef __rho_geo_tLine_h__
#define __rho_geo_tLine_h__


#include <rho/geo/tVector.h>


namespace rho
{
namespace geo
{


class tLine
{
    public:

        tLine();

        tLine(double x,  double y,
              double dx, double dy);

        tLine(double x,  double y,  double z,
              double dx, double dy, double dz);

        tLine(const tVector& p, const tVector& v);

    public:

        tVector p;    // a point on the line
        tVector v;    // the line's direction vector
};


}   // namespace geo
}   // namespace rho


#endif    // __rho_geo_tLine_h__
