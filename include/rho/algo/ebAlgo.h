#ifndef __rho_algo_ebAlgo_h__
#define __rho_algo_ebAlgo_h__


#include <rho/ppcheck.h>
#include <rho/eRho.h>


namespace rho
{
namespace algo
{


class ebAlgo : public ebObject
{
    public:

        ebAlgo(std::string reason) : ebObject(reason) { }

        ~ebAlgo() throw() { }
};


}    // namespace algo
}    // namespace rho


#endif    // __rho_algo_ebAlgo_h__
