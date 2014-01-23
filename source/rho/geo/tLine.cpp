#include <rho/geo/tLine.h>


namespace rho
{
namespace geo
{


tLine::tLine()
    : p(tVector::origin()),       // point on the line
      v(0.0, 0.0, 0.0)            // the line's direction vector
{
}

tLine::tLine(f64 x,  f64 y,
             f64 dx, f64 dy)
    : p(tVector::point(x, y)),
      v(dx, dy)
{
}

tLine::tLine(f64 x,  f64 y,  f64 z,
             f64 dx, f64 dy, f64 dz)
    : p(tVector::point(x, y, z)),
      v(dx, dy, dz)
{
}

tLine::tLine(const tVector& p, const tVector& v)
    : p(p),
      v(v)
{
}


void pack(iWritable* out, const tLine& line)
{
    pack(out, line.p);
    pack(out, line.v);
}

void unpack(iReadable* in, tLine& line)
{
    unpack(in, line.p);
    unpack(in, line.v);
}


}    // namespace geo
}    // namespace rho
