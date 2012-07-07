#ifndef __rho_tStacktrace_h__
#define __rho_tStacktrace_h__


#include <execinfo.h>

#include <cstdlib>
#include <ostream>


namespace rho
{


class tStacktrace
{
    public:

        tStacktrace();

        void print(std::ostream& o) const;

    private:

        static const int kCallStackMaxSize = 100;

        int   m_numFrames;
        void* m_returnPtrs[kCallStackMaxSize];
};


tStacktrace::tStacktrace()
{
    m_numFrames = backtrace(m_returnPtrs, kCallStackMaxSize);
}

void tStacktrace::print(std::ostream& o) const
{
    char** frameStrs = backtrace_symbols(m_returnPtrs, m_numFrames);
    for (int i = 0; i < m_numFrames; i++)
        o << frameStrs[i] << std::endl;
    free(frameStrs);
}


}    // namespace rho


#endif   // __rho_tStacktrace_h__
