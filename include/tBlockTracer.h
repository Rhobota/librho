#ifndef __rho_tBlockTracer_h__
#define __rho_tBlockTracer_h__


#include "bNonCopyable.h"
#include "types.h"

#include "sync/tThreadLocal.h"

#include <iostream>
#include <stack>
#include <string>


namespace rho
{


#define RHO_BT() \
    rho::tBlockTracer _curr_bl_(__PRETTY_FUNCTION__);


#define RHO_VAR(x) \
    { \
    rho::tBlockTracer* _top_bl_ = rho::tBlockTracer::top(); \
    std::ostream& _top_bl_ostream_ = \
                        (_top_bl_) ? (_top_bl_->getStream()) : (std::cerr); \
    rho::tBlockTracer::printIndent(_top_bl_ostream_); \
    _top_bl_ostream_ << #x << " == " << x << "\n"; \
    }


#define RHO_LOG(x) \
    { \
    rho::tBlockTracer* _top_bl_ = rho::tBlockTracer::top(); \
    std::ostream& _top_bl_ostream_ = \
                        (_top_bl_) ? (_top_bl_->getStream()) : (std::cerr); \
    rho::tBlockTracer::printIndent(_top_bl_ostream_); \
    _top_bl_ostream_ << x << "\n"; \
    }


class tBlockTracer : public bNonCopyable
{
    public:

        tBlockTracer(std::string blockName);

        tBlockTracer(std::string blockName, std::ostream& o);

        ~tBlockTracer();

    public:

        static void printIndent(std::ostream& o);

        static tBlockTracer* top();

        std::ostream& getStream();

    private:

        void m_init();

    private:

        std::string   m_blockName;
        std::ostream& m_ostream;

    private:

        static u16                     m_indentWidth;
        static sync::tThreadLocal<u32> m_indentCount;

        static sync::tThreadLocal< std::stack<tBlockTracer*> > m_btStack;
};


u16                     tBlockTracer::m_indentWidth = 4;
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


#endif   // __rho_tBlockTracer_h__
