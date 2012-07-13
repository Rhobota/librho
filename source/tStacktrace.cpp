#include "rho/tStacktrace.h"

#include <execinfo.h>


namespace rho
{


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
