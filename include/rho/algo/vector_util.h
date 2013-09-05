#ifndef __rho_algo_vector_util_h__
#define __rho_algo_vector_util_h__


#include <rho/eRho.h>

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
void unzip(const std::vector< std::pair<A,B> >& zipped, vector<A>& firstPart,
                                                        vector<B>& secondPart);

template <class A>
void shuffle(std::vector<A>& v);

template <class A>
std::vector<A> mix(const std::vector<A>& a,
                   const std::vector<A>& b);


}   // namespace algo
}   // namespace rho


#include "vector_util.ipp"


#endif   // __rho_algo_vector_util_h__
