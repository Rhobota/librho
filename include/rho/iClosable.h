#ifndef __rho_iClosable_h__
#define __rho_iClosable_h__


#include <rho/ppcheck.h>


namespace rho
{


class iClosable
{
    public:

        virtual void close() = 0;

        virtual ~iClosable() { }
};


}   // namespace rho


#endif    // __rho_iClosable_h__
