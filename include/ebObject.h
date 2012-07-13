#ifndef __rho_ebObject_h__
#define __rho_ebObject_h__


#include "tStacktrace.h"

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


ebObject::ebObject(std::string reason)
    : m_reason(reason),
      m_stacktrace()
{
}

std::string ebObject::reason() const
{
    return m_reason;
}

const char* ebObject::what() const throw()
{
    return m_reason.c_str();
}

void ebObject::printStacktrace(std::ostream& o) const
{
    m_stacktrace.print(o);
}

ebObject::~ebObject() throw()
{
}


}   // namespace rho


#endif    // __rho_ebObject_h__
