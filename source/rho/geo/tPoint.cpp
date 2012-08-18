#include <rho/geo/tPoint.h>


namespace rho
{
namespace geo
{


tPoint::tPoint()
{
    x = y = 0.0;
}

tPoint::tPoint(double x, double y)
{
    this->x = x;
    this->y = y;
}


}    // namespace geo
}    // namespace rho
