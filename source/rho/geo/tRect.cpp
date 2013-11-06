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
    normalize();
}

void tRect::beNull()
{
    x = y = width = height = NAN;
}

bool tRect::isNull() const
{
    return std::isnan(x) ||
           std::isnan(y) ||
           std::isnan(width) ||
           std::isnan(height);
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
    o.normalize();
    o.x += dx;
    o.width -= 2*dx;
    o.y += dy;
    o.height -= 2*dy;
    if (o.width < 0.0)
    {
        o.x += o.width/2;
        o.width = 0.0;
    }
    if (o.height < 0.0)
    {
        o.y += o.height/2;
        o.height = 0.0;
    }
    return o;
}

tRect tRect::intersect(tRect other) const
{
    if (isNull())
        return *this;
    if (other.isNull())
        return other;

    tRect r = *this;
    r.normalize();
    other.normalize();

    tRect inter(std::max(r.x, other.x),
                std::max(r.y, other.y),
                std::min(r.x + r.width, other.x + other.width),
                std::min(r.y + r.height, other.y + other.height));
    inter.width -= inter.x;
    inter.height -= inter.y;

    if (inter.width < 0.0 || inter.height < 0.0)
        inter.beNull();

    return inter;
}

tRect tRect::unionn(tRect other) const
{
    if (isNull())
        return other;
    if (other.isNull())
        return *this;

    tRect r = *this;
    r.normalize();
    other.normalize();

    tRect un(std::min(r.x, other.x),
             std::min(r.y, other.y),
             std::max(r.x + r.width, other.x + other.width),
             std::max(r.y + r.height, other.y + other.height));
    un.width -= un.x;
    un.height -= un.y;

    return un;
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

tBox tRect::toBox() const
{
    return tBox(x, y, x+width, y+height);
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
