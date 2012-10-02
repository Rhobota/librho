#include <cmath>


namespace rho
{
namespace geo
{


const double kPI = acos(-1.0);


double toRadians(double angle)
{
    return angle * kPI / 180.0;
}


double toDegrees(double angle)
{
    return angle * 180.0 / kPI;
}


}
}
