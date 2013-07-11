#include <rho/ml/common.h>


namespace rho
{
namespace ml
{


f64 logistic_function(f64 z)
{
    return (1.0 / (1.0 + std::exp(-z)));
}

f64 derivative_of_logistic_function(f64 z)
{
    f64 y = logistic_function(z);
    f64 slope = (y * (1.0 - y));
    slope = std::max(slope, 1e-5);    // <-- Experimental
    return slope;
}

f64 logistic_function_min()
{
    return 0.0;
}

f64 logistic_function_max()
{
    return 1.0;
}


}   // namespace ml
}   // namespace rho
