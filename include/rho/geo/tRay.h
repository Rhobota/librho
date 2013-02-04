#ifndef __rho_geo_tRay_h__
#define __rho_geo_tRay_h__


#include <rho/iPackable.h>
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

        tRay(f64 x,  f64 y,
             f64 dx, f64 dy);

        tRay(f64 x,  f64 y,  f64 z,
             f64 dx, f64 dy, f64 dz);

        tRay(const tVector& p, const tVector& v);

        tBox  toBox()  const;
        tLine toLine() const;

    public:

        tVector p;    // the origin point of the ray
        tVector v;    // the ray's direction vector
};


void pack(iWritable* out, const tRay&);
void unpack(iReadable* in, tRay&);


}   // namespace geo
}   // namespace rho


#endif    // __rho_geo_tRay_h__
