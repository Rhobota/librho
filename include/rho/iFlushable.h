#ifndef __rho_iFlushable_h__
#define __rho_iFlushable_h__


#include <rho/ppcheck.h>


namespace rho
{


class iFlushable
{
    public:

        virtual bool flush() = 0;

        virtual ~iFlushable() { }
};


}     // namespace rho


#endif    // __rho_iFlushable_h__
