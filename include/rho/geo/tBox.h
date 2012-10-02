#ifndef __rho_geo_tBox_h__
#define __rho_geo_tBox_h__


#include <rho/geo/tVector.h>


namespace rho
{
namespace geo
{


class tBox
{
    public:

        tBox();

        tBox(const tVector& p1, const tVector& p2);

        tBox(double x1, double y1,
             double x2, double y2);

        tBox(double x1, double y1, double z1,
             double x2, double y2, double z2);

        tBox& operator* (double a);

        tBox& operator/ (double a);

    public:

        tVector p1, p2;
};


}    // namespace geo
}    // namespace rho


#endif    // __rho_geo_tBox_h__
