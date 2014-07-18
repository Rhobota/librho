#ifndef __rho_geo_units_h__
#define __rho_geo_units_h__


#include <rho/ppcheck.h>
#include <rho/types.h>


namespace rho
{
namespace geo
{


extern const f64 kPI;

f64 toRadians(f64 angle);
f64 toDegrees(f64 angle);


}
}


#endif  // __rho_geo_units_h__
