#ifndef __rho_bObjectTracer_h__
#define __rho_bObjectTracer_h__


#include <rho/ppcheck.h>
#include <iostream>
#include <string>


namespace rho
{


class bObjectTracer
{
    public:

        bObjectTracer(std::string subClassName);

        bObjectTracer(std::string subClassName, std::ostream& o);

        bObjectTracer(const bObjectTracer& other);

        const bObjectTracer& operator= (const bObjectTracer& other);

        ~bObjectTracer();

    private:

        std::string   m_subClassName;
        std::ostream& m_ostream;
};


}      // namespace rho


#endif    // __rho_bObjectTracer_h__
