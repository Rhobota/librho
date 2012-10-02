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

tLine::tLine(double x,  double y,
             double dx, double dy)
    : p(tVector::point(x, y)),
      v(dx, dy)
{
}

tLine::tLine(double x,  double y,  double z,
             double dx, double dy, double dz)
    : p(tVector::point(x, y, z)),
      v(dx, dy, dz)
{
}

tLine::tLine(const tVector& p, const tVector& v)
    : p(p),
      v(v)
{
}


}    // namespace geo
}    // namespace rho
