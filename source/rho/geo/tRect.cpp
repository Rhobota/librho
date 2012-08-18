#include <rho/geo/tRect.h>


namespace rho
{
namespace geo
{


tRect::tRect()
{
    x = y = width = height = 0.0;
}

tRect::tRect(double x, double y,
      double width, double height)
{
    this->x = x;
    this->y = y;
    this->width = width;
    this->height = height;
}

tRect& tRect::operator* (double a)
{
    x *= a;
    y *= a;
    width *= a;
    height *= a;
    return *this;
}

tRect& tRect::operator/ (double a)
{
    return (*this) * (1.0/a);
}


}    // namespace geo
}    // namespace rho
