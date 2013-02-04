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

tRay::tRay(f64 x,  f64 y,
           f64 dx, f64 dy)
    : p(tVector::point(x, y)),
      v(dx, dy)
{
}

tRay::tRay(f64 x,  f64 y,  f64 z,
           f64 dx, f64 dy, f64 dz)
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
    f64 x1 = std::min(p.x, p.x + v.x);
    f64 y1 = std::min(p.y, p.y + v.y);
    f64 z1 = std::min(p.z, p.z + v.z);
    f64 x2 = std::max(p.x, p.x + v.x);
    f64 y2 = std::max(p.y, p.y + v.y);
    f64 z2 = std::max(p.z, p.z + v.z);
    return tBox(x1, y1, z1, x2, y2, z2);
}

tLine tRay::toLine() const
{
    return tLine(p, v);
}


void pack(iWritable* out, const tRay& ray)
{
    pack(out, ray.p);
    pack(out, ray.v);
}

void unpack(iReadable* in, tRay& ray)
{
    unpack(in, ray.p);
    unpack(in, ray.v);
}


}    // namespace geo
}    // namespace rho
