#ifndef __rho_geo_tRect_h__
#define __rho_geo_tRect_h__


#include <rho/ppcheck.h>
#include <rho/iDrawable.h>
#include <rho/iPackable.h>
#include <rho/geo/tBox.h>


namespace rho
{
namespace geo
{


class tRect : public bDrawableByArtist
{
    public:

        tRect();

        tRect(f64 x,     f64 y,
              f64 width, f64 height);

        void beNull();
        bool isNull() const;

        void normalize();

        tRect inset(f64 dx, f64 dy) const;

        tRect intersect(tRect other) const;
        tRect unionn(tRect other) const;

        tRect operator* (f64 a) const;
        tRect operator/ (f64 a) const;

        tBox toBox() const;

        void drawWithArtist(iArtist& artist);

    public:

        f64 x, y, width, height;
};


void pack(iWritable* out, const tRect&);
void unpack(iReadable* in, tRect&);


}    // namespace geo
}    // namespace rho


#endif    // __rho_geo_tRect_h__
