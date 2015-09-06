#include <rho/geo/units.h>

#include <cmath>


namespace rho
{
namespace geo
{


const f64 kPI = acos(-1.0);


f64 toRadians(f64 angle)
{
    return angle * kPI / 180.0;
}


f64 toDegrees(f64 angle)
{
    return angle * 180.0 / kPI;
}


}
}
