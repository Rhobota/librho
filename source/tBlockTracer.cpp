#include "rho/tBlockTracer.h"

#include "rho/sync/tThreadLocal.h"


namespace rho
{


const u16               tBlockTracer::m_indentWidth = 4;
sync::tThreadLocal<u32> tBlockTracer::m_indentCount;

sync::tThreadLocal< std::stack<tBlockTracer*> > tBlockTracer::m_btStack;


tBlockTracer::tBlockTracer(std::string blockName)
    : m_blockName(blockName),
      m_ostream(std::cerr)
{
    m_init();
}

tBlockTracer::tBlockTracer(std::string blockName, std::ostream& o)
    : m_blockName(blockName),
      m_ostream(o)
{
    m_init();
}

void tBlockTracer::m_init()
{
    if (! m_indentCount)
        m_indentCount = new u32(0);
    if (! m_btStack)
        m_btStack = new std::stack<tBlockTracer*>();

    printIndent(m_ostream);
    m_ostream << m_blockName << "\n";

    printIndent(m_ostream);
    m_ostream << "{\n";

    ++(*m_indentCount);
    m_btStack->push(this);
}

tBlockTracer::~tBlockTracer()
{
    --(*m_indentCount);
    m_btStack->pop();

    printIndent(m_ostream);
    m_ostream << "}\n";

    if (*m_indentCount == 0)
    {
        m_indentCount = NULL;
        m_btStack = NULL;
    }
}

void tBlockTracer::printIndent(std::ostream& o)
{
    u32 indentCount = 0;
    if (m_indentCount)
        indentCount = *m_indentCount;

    u64 indentAmount = indentCount * m_indentWidth;
    for (u64 i = 0; i < indentAmount; ++i)
        o << " ";
}

tBlockTracer* tBlockTracer::top()
{
    if (! m_btStack)
        return NULL;

    if (m_btStack->size() > 0)
        return m_btStack->top();
    else
        return NULL;
}

std::ostream& tBlockTracer::getStream()
{
    return m_ostream;
}


}   // namespace rho
