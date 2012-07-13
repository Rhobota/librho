#include "rho/ebObject.h"


namespace rho
{


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
