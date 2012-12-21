#ifndef __rho_iFlushable_h__
#define __rho_iFlushable_h__


namespace rho
{


class iFlushable
{
    public:

        virtual void flush() = 0;

        virtual ~iFlushable() { }
};


}     // namespace rho


#endif    // __rho_iFlushable_h__
