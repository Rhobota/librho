#ifndef __rho_geo_tRect_h__
#define __rho_geo_tRect_h__


#include <rho/geo/tBox.h>


namespace rho
{
namespace geo
{


class tRect
{
    public:

        tRect();

        tRect(double x,     double y,
              double width, double height);

        tBox toBox() const;

        tRect operator* (double a) const;
        tRect operator/ (double a) const;

    public:

        double x, y, width, height;
};


}    // namespace geo
}    // namespace rho


#endif    // __rho_geo_tRect_h__
