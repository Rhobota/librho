#ifndef __rho_geo_tRect_h__
#define __rho_geo_tRect_h__


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

        tRect operator* (double a) const;
        tRect operator/ (double a) const;

    public:

        double x, y, width, height;
};


}    // namespace geo
}    // namespace rho


#endif    // __rho_geo_tRect_h__
