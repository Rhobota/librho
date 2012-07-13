#ifndef __rho_iOutputStream_h__
#define __rho_iOutputStream_h__


#include "types.h"


namespace rho
{


class iOutputStream
{
    public:

        virtual int write(const u8* buffer, int length) = 0;

        virtual ~iOutputStream() { }
};


}    // namespace rho


#endif    // __rho_iOutputStream_h__
