#ifndef __rho_algo_vector_util_h__
#define __rho_algo_vector_util_h__


#include <rho/ppcheck.h>
#include <rho/eRho.h>
#include <rho/algo/tLCG.h>

#include <vector>
#include <utility>


namespace rho
{
namespace algo
{


template <class A, class B>
std::vector< std::pair<A,B> > zip(const std::vector<A>& a,
                                  const std::vector<B>& b);

template <class A, class B>
std::vector< std::pair<A,B> > zip(const std::vector<A>& a,
                                  const B& b);

template <class A, class B>
void unzip(const std::vector< std::pair<A,B> >& zipped, std::vector<A>& firstPart,
                                                        std::vector<B>& secondPart);

template <class A>
void shuffle(std::vector<A>& v, iLCG& lcg = gDefaultLCG);

template <class A, class B>
void shuffle(std::vector<A>& a, std::vector<B>& b, iLCG& lcg = gDefaultLCG);   // <-- shuffles a and b in the same way

template <class A>
std::vector<A> mix(const std::vector<A>& a,
                   const std::vector<A>& b,
                   iLCG& lcg = gDefaultLCG);


}   // namespace algo
}   // namespace rho


#include "vector_util.ipp"


#endif   // __rho_algo_vector_util_h__
