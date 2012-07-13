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

        static const u16               m_indentWidth;
        static sync::tThreadLocal<u32> m_indentCount;

        static sync::tThreadLocal< std::stack<tBlockTracer*> > m_btStack;
};


}   // namespace rho


#endif   // __rho_tBlockTracer_h__
