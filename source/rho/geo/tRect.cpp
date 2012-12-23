#include <rho/geo/tRect.h>


namespace rho
{
namespace geo
{


tRect::tRect()
    : x(0.0), y(0.0), width(0.0), height(0.0)
{
}

tRect::tRect(double x,     double y,
             double width, double height)
    : x(x), y(y), width(width), height(height)
{
}

tBox tRect::toBox() const
{
    return tBox(x, y, x+width, y+height);
}

tRect tRect::operator* (double a) const
{
    tRect r = (*this);
    r.x *= a;
    r.y *= a;
    r.width *= a;
    r.height *= a;
    return r;
}

tRect tRect::operator/ (double a) const
{
    return (*this) * (1.0/a);
}

void tRect::drawWithArtist(iArtist& artist)
{
    artist.drawRect(*this);
}


}    // namespace geo
}    // namespace rho
