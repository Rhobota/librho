#ifndef __rho_geo_tBox_h__
#define __rho_geo_tBox_h__


#include <rho/iDrawable.h>
#include <rho/iPackable.h>
#include <rho/geo/tVector.h>


namespace rho
{
namespace geo
{


class tBox : public bDrawableByArtist
{
    public:

        tBox();

        tBox(const tVector& p1, const tVector& p2);

        tBox(f64 x1, f64 y1,
             f64 x2, f64 y2);

        tBox(f64 x1, f64 y1, f64 z1,
             f64 x2, f64 y2, f64 z2);

        tBox normalize() const;

        tBox operator* (f64 a) const;
        tBox operator/ (f64 a) const;

    public:

        tVector p1, p2;
};


void pack(iWritable* out, const tBox&);
void unpack(iReadable* in, tBox&);


}    // namespace geo
}    // namespace rho


#endif    // __rho_geo_tBox_h__
