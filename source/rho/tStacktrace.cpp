#include <rho/tStacktrace.h>

#if __linux__ || __APPLE__
#include <execinfo.h>
#endif


namespace rho
{


tStacktrace::tStacktrace()
    : m_numFrames(0)
{
    #if __linux__ || __APPLE__
    m_numFrames = backtrace(m_returnPtrs, kCallStackMaxSize);
    #endif
}

void tStacktrace::print(std::ostream& o) const
{
    #if __linux__ || __APPLE__
    char** frameStrs = backtrace_symbols(m_returnPtrs, m_numFrames);
    for (int i = 0; i < m_numFrames; i++)
        o << frameStrs[i] << std::endl;
    free(frameStrs);
    #else
    o << "Stacktraces are not supported on this platform." << std::endl;
    #endif
}


}    // namespace rho
