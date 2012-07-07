#ifndef __rho_tBlockTracer_h__
#define __rho_tBlockTracer_h__


#include "bNonCopyable.h"
#include "types.h"

#include <iostream>
#include <string>


namespace rho
{


class tBlockTracer : public bNonCopyable
{
    public:

        tBlockTracer(std::string blockName);

        tBlockTracer(std::string blockName, std::ostream& o);

        ~tBlockTracer();

    private:

        std::string   m_blockName;
        std::ostream& m_ostream;

    private:

        void printIndent();

        static u16 m_sIndentWidth;
        static u32 m_sIndentCount;
};


u16 tBlockTracer::m_sIndentWidth = 4;
u32 tBlockTracer::m_sIndentCount = 0;


tBlockTracer::tBlockTracer(std::string blockName)
    : m_blockName(blockName),
      m_ostream(std::cerr)
{
    printIndent();
    m_ostream << m_blockName << "\n";
    printIndent();
    m_ostream << "{\n";
    ++m_sIndentCount;
}

tBlockTracer::tBlockTracer(std::string blockName, std::ostream& o)
    : m_blockName(blockName),
      m_ostream(o)
{
    printIndent();
    m_ostream << m_blockName << "\n";
    printIndent();
    m_ostream << "{\n";
    ++m_sIndentCount;
}

tBlockTracer::~tBlockTracer()
{
    --m_sIndentCount;
    printIndent();
    m_ostream << "}\n";
}

void tBlockTracer::printIndent()
{
    u64 indentAmount = m_sIndentCount * m_sIndentWidth;
    for (u64 i = 0; i < indentAmount; ++i)
        m_ostream << " ";
}


}   // namespace rho


#endif   // __rho_tBlockTracer_h__
