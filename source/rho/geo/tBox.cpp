#include <rho/geo/tBox.h>


namespace rho
{
namespace geo
{


tBox::tBox()
    : p1(tVector::origin()),
      p2(tVector::origin())
{
}

tBox::tBox(const tVector& p1, const tVector& p2)
    : p1(p1),
      p2(p2)
{
    if (!p1.isPoint())
        throw rho::eLogicError("P1 must be a point!");
    if (!p2.isPoint())
        throw rho::eLogicError("P2 must be a point!");
}

tBox::tBox(f64 x1, f64 y1,
           f64 x2, f64 y2)
    : p1(tVector::origin()),
      p2(tVector::origin())
{
    p1.x = x1;
    p1.y = y1;
    p2.x = x2;
    p2.y = y2;
}

tBox::tBox(f64 x1, f64 y1, f64 z1,
           f64 x2, f64 y2, f64 z2)
    : p1(tVector::origin()),
      p2(tVector::origin())
{
    p1.x = x1;
    p1.y = y1;
    p1.z = z1;
    p2.x = x2;
    p2.y = y2;
    p2.z = z2;
}

tBox tBox::normalize() const
{
    f64 x1 = std::min(p1.x, p2.x);
    f64 y1 = std::min(p1.y, p2.y);
    f64 z1 = std::min(p1.z, p2.z);
    f64 x2 = std::max(p1.x, p2.x);
    f64 y2 = std::max(p1.y, p2.y);
    f64 z2 = std::max(p1.z, p2.z);
    return tBox(x1, y1, z1, x2, y2, z2);
}

tBox tBox::operator* (f64 a) const
{
    tBox b = (*this);
    b.p1 *= a;
    b.p2 *= a;
    return b;
}

tBox tBox::operator/ (f64 a) const
{
    return (*this) * (1.0/a);
}


void pack(iWritable* out, const tBox& box)
{
    pack(out, box.p1);
    pack(out, box.p2);
}

void unpack(iReadable* in, tBox& box)
{
    unpack(in, box.p1);
    unpack(in, box.p2);
}


}    // namespace geo
}    // namespace rho
