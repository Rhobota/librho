#include <rho/geo/tRect.h>


namespace rho
{
namespace geo
{


tRect::tRect()
    : x(0.0), y(0.0), width(0.0), height(0.0)
{
}

tRect::tRect(f64 x,     f64 y,
             f64 width, f64 height)
    : x(x), y(y), width(width), height(height)
{
}

void tRect::normalize()
{
    if (width < 0.0)
    {
        x += width;
        width = -width;
    }
    if (height < 0.0)
    {
        y += height;
        height = -height;
    }
}

tRect tRect::inset(f64 dx, f64 dy) const
{
    tRect o = *this;
    o.x += dx;
    o.width -= 2*dx;
    o.y += dy;
    o.height -= 2*dy;
    return o;
}

tBox tRect::toBox() const
{
    return tBox(x, y, x+width, y+height);
}

tRect tRect::operator* (f64 a) const
{
    tRect r = (*this);
    r.x *= a;
    r.y *= a;
    r.width *= a;
    r.height *= a;
    return r;
}

tRect tRect::operator/ (f64 a) const
{
    return (*this) * (1.0/a);
}


void pack(iWritable* out, const tRect& rect)
{
    pack(out, rect.x);
    pack(out, rect.y);
    pack(out, rect.width);
    pack(out, rect.height);
}

void unpack(iReadable* in, tRect& rect)
{
    unpack(in, rect.x);
    unpack(in, rect.y);
    unpack(in, rect.width);
    unpack(in, rect.height);
}


}    // namespace geo
}    // namespace rho
