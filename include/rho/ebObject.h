#ifndef __rho_ebObject_h__
#define __rho_ebObject_h__


#include <rho/ppcheck.h>
#include <rho/tStacktrace.h>

#include <cstdlib>
#include <exception>
#include <ostream>
#include <stdexcept>
#include <string>


namespace rho
{


class ebObject : public std::exception
{
    public:

        ebObject(std::string reason);

        virtual std::string reason() const;

        virtual const char* what() const throw();   // std::exception override

        void printStacktrace(std::ostream& o) const;

        ~ebObject() throw();

    private:

        std::string m_reason;
        tStacktrace m_stacktrace;
};


}   // namespace rho


#endif    // __rho_ebObject_h__
