#ifndef __rho_algo_ebAlgo_h__
#define __rho_algo_ebAlgo_h__


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


class eInvalidArgument : public ebAlgo
{
    public:

        eInvalidArgument(std::string reason) : ebAlgo(reason) { }

        ~eInvalidArgument() throw() { }
};


}    // namespace algo
}    // namespace rho


#endif    // __rho_algo_ebAlgo_h__
