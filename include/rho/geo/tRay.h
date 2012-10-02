#ifndef __rho_geo_tRay_h__
#define __rho_geo_tRay_h__


#include <rho/geo/tBox.h>
#include <rho/geo/tLine.h>
#include <rho/geo/tVector.h>


namespace rho
{
namespace geo
{


class tRay
{
    public:

        tRay();

        tRay(double x,  double y,
             double dx, double dy);

        tRay(double x,  double y,  double z,
             double dx, double dy, double dz);

        tRay(const tVector& p, const tVector& v);

        tBox  toBox()  const;
        tLine toLine() const;

    public:

        tVector p;    // the origin point of the ray
        tVector v;    // the ray's direction vector
};


}   // namespace geo
}   // namespace rho


#endif    // __rho_geo_tRay_h__
