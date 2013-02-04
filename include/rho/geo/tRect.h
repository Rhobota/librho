#ifndef __rho_geo_tRect_h__
#define __rho_geo_tRect_h__


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

        tBox toBox() const;

        tRect operator* (f64 a) const;
        tRect operator/ (f64 a) const;

        void drawWithArtist(iArtist& artist);

    public:

        f64 x, y, width, height;
};


void pack(iWritable* out, const tRect&);
void unpack(iReadable* in, tRect&);


}    // namespace geo
}    // namespace rho


#endif    // __rho_geo_tRect_h__
