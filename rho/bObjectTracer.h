#ifndef __rho_bObjectTracer_h__
#define __rho_bObjectTracer_h__


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


bObjectTracer::bObjectTracer(std::string subClassName)
    : m_subClassName(subClassName),
      m_ostream(std::cerr)
{
    m_ostream << m_subClassName << "()" << std::endl;
}

bObjectTracer::bObjectTracer(std::string subClassName, std::ostream& o)
    : m_subClassName(subClassName),
      m_ostream(o)
{
    m_ostream << m_subClassName << "()" << std::endl;
}

bObjectTracer::bObjectTracer(const bObjectTracer& other)
    : m_subClassName(other.m_subClassName),
      m_ostream(other.m_ostream)
{
    m_ostream << m_subClassName << "(const " << m_subClassName << "&)"
              << std::endl;
}

const bObjectTracer& bObjectTracer::operator= (const bObjectTracer& other)
{
    m_ostream << "operator= (" << m_subClassName << "&, "
              << "const " << other.m_subClassName << "&)" << std::endl;
    m_subClassName = other.m_subClassName;
    return *this;
}

bObjectTracer::~bObjectTracer()
{
    m_ostream << "~" << m_subClassName << "()" << std::endl;
}


}      // namespace rho


#endif    // __rho_bObjectTracer_h__
