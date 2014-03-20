#ifndef __rho_algo_stat_util_h__
#define __rho_algo_stat_util_h__


#include <rho/types.h>
#include <rho/algo/tLCG.h>

#include <cmath>
#include <vector>


namespace rho
{
namespace algo
{


/////////////////////////////////////////////////////////////
// Calculating the mean of a vector or matrix:
/////////////////////////////////////////////////////////////

template <class T>
T mean(const std::vector<T>& v);

template <class T>
T mean(const std::vector< std::vector<T> >& m);


/////////////////////////////////////////////////////////////
// Calculating the variance of a vector or matrix:
/////////////////////////////////////////////////////////////

template <class T>
T variance(const std::vector<T>& v);

template <class T>
T variance(const std::vector< std::vector<T> >& m);


/////////////////////////////////////////////////////////////
// Calculating the standard deviation of a vector or matrix:
/////////////////////////////////////////////////////////////

template <class T>
T stddev(const std::vector<T>& v);

template <class T>
T stddev(const std::vector< std::vector<T> >& m);


/////////////////////////////////////////////////////////////
// Generating pseudo-random numbers from the standard
// normal distribution (mean of zero and stddev of 1):
/////////////////////////////////////////////////////////////

f64 nrand(iLCG& lcg = gKnuthLCG);


/////////////////////////////////////////////////////////////
// END
/////////////////////////////////////////////////////////////

#include "stat_util.ipp"


}  // namespace algo
}  // namespace rho


#endif // __rho_algo_stat_util_h__
