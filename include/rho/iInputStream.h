#ifndef __rho_iInputStream_h__
#define __rho_iInputStream_h__


#include <rho/types.h>


namespace rho
{


class iInputStream
{
    public:

        virtual int read(u8* buffer, int length) = 0;

        virtual ~iInputStream() { }
};


}     // namespace rho


#endif    // __rho_iInputStream_h__
