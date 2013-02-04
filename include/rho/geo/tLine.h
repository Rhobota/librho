#ifndef __rho_geo_tLine_h__
#define __rho_geo_tLine_h__


#include <rho/iPackable.h>
#include <rho/geo/tVector.h>


namespace rho
{
namespace geo
{


class tLine
{
    public:

        tLine();

        tLine(f64 x,  f64 y,
              f64 dx, f64 dy);

        tLine(f64 x,  f64 y,  f64 z,
              f64 dx, f64 dy, f64 dz);

        tLine(const tVector& p, const tVector& v);

    public:

        tVector p;    // a point on the line
        tVector v;    // the line's direction vector
};


void pack(iWritable* out, const tLine&);
void unpack(iReadable* in, tLine&);


}   // namespace geo
}   // namespace rho


#endif    // __rho_geo_tLine_h__
