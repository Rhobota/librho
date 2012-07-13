#include "rho/bObjectTracer.h"
#include "rho/tBlockTracer.h"


namespace rho
{


bObjectTracer::bObjectTracer(std::string subClassName)
    : m_subClassName(subClassName),
      m_ostream(std::cerr)
{
    tBlockTracer::printIndent(m_ostream);
    m_ostream << m_subClassName << "()" << std::endl;
}

bObjectTracer::bObjectTracer(std::string subClassName, std::ostream& o)
    : m_subClassName(subClassName),
      m_ostream(o)
{
    tBlockTracer::printIndent(m_ostream);
    m_ostream << m_subClassName << "()" << std::endl;
}

bObjectTracer::bObjectTracer(const bObjectTracer& other)
    : m_subClassName(other.m_subClassName),
      m_ostream(other.m_ostream)
{
    tBlockTracer::printIndent(m_ostream);
    m_ostream << m_subClassName << "(const " << m_subClassName << "&)"
              << std::endl;
}

const bObjectTracer& bObjectTracer::operator= (const bObjectTracer& other)
{
    tBlockTracer::printIndent(m_ostream);
    m_ostream << "operator= (" << m_subClassName << "&, "
              << "const " << other.m_subClassName << "&)" << std::endl;
    m_subClassName = other.m_subClassName;
    return *this;
}

bObjectTracer::~bObjectTracer()
{
    tBlockTracer::printIndent(m_ostream);
    m_ostream << "~" << m_subClassName << "()" << std::endl;
}


}      // namespace rho
