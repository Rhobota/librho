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

tBox::tBox(double x1, double y1,
           double x2, double y2)
    : p1(tVector::origin()),
      p2(tVector::origin())
{
    p1.x = x1;
    p1.y = y1;
    p2.x = x2;
    p2.y = y2;
}

tBox::tBox(double x1, double y1, double z1,
           double x2, double y2, double z2)
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
    double x1 = std::min(p1.x, p2.x);
    double y1 = std::min(p1.y, p2.y);
    double z1 = std::min(p1.z, p2.z);
    double x2 = std::max(p1.x, p2.x);
    double y2 = std::max(p1.y, p2.y);
    double z2 = std::max(p1.z, p2.z);
    return tBox(x1, y1, z1, x2, y2, z2);
}

tBox tBox::operator* (double a) const
{
    tBox b = (*this);
    b.p1 *= a;
    b.p2 *= a;
    return b;
}

tBox tBox::operator/ (double a) const
{
    return (*this) * (1.0/a);
}


}    // namespace geo
}    // namespace rho
