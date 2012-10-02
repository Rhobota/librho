#include <rho/geo/tRay.h>

#include <algorithm>


namespace rho
{
namespace geo
{


tRay::tRay()
    : p(tVector::origin()),       // origin point of the ray
      v(0.0, 0.0, 0.0)            // the ray's direction vector
{
}

tRay::tRay(double x,  double y,
           double dx, double dy)
    : p(tVector::point(x, y)),
      v(dx, dy)
{
}

tRay::tRay(double x,  double y,  double z,
           double dx, double dy, double dz)
    : p(tVector::point(x, y, z)),
      v(dx, dy, dz)
{
}

tRay::tRay(const tVector& p, const tVector& v)
    : p(p),
      v(v)
{
}

tBox tRay::toBox() const
{
    double x1 = std::min(p.x, p.x + v.x);
    double y1 = std::min(p.y, p.y + v.y);
    double z1 = std::min(p.z, p.z + v.z);
    double x2 = std::max(p.x, p.x + v.x);
    double y2 = std::max(p.y, p.y + v.y);
    double z2 = std::max(p.z, p.z + v.z);
    return tBox(x1, y1, z1, x2, y2, z2);
}

tLine tRay::toLine() const
{
    return tLine(p, v);
}


}    // namespace geo
}    // namespace rho
