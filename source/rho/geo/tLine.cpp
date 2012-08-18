#include <rho/geo/tLine.h>

#include <algorithm>


namespace rho
{
namespace geo
{


tLine::tLine()
    : p1(), p2()
{
}

tRect tLine::rect()
{
    double x1 = std::min(p1.x, p2.x);
    double y1 = std::min(p1.y, p2.y);
    double x2 = std::max(p1.x, p2.x);
    double y2 = std::max(p1.y, p2.y);
    return tRect(x1, y1, x2-x1, y2-y1);
}


}    // namespace geo
}    // namespace rho
