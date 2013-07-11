#ifndef __rho_ml_common_h__
#define __rho_ml_common_h__


#include <rho/eRho.h>
#include <rho/types.h>

#include <cmath>
#include <iostream>


namespace rho
{
namespace ml
{


f64 logistic_function(f64 z);
f64 derivative_of_logistic_function(f64 z);

f64 logistic_function_min();
f64 logistic_function_max();


}    // namespace ml
}    // namespace rho


#endif   // __rho_ml_common_h__
