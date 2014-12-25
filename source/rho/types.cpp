#include <rho/types.h>
#include <rho/eRho.h>

#ifdef __APPLE__
#include "TargetConditionals.h"
#endif

#include <sstream>
#include <errno.h>
#include <string.h>

#if __linux__ || __APPLE__ || __CYGWIN__
#include <sys/utsname.h>
#elif __MINGW32__
#include <windows.h>
#else
#error What platform are you on!?
#endif


namespace rho
{


nPlatform getCurrPlatform()
{
    #if __linux__
        return kLinux;
    #elif __APPLE__
        #if !TARGET_OS_IPHONE && !TARGET_IPHONE_SIMULATOR
            return kOSX;
        #else
            return kIOS;
        #endif
    #elif __CYGWIN__
        return kCygwin;
    #elif __MINGW32__
        return kWindows;
    #else
        #error Unsupported platform
    #endif
}


std::string platformEnumToString(nPlatform plat)
{
    switch (plat)
    {
        case kLinux:
            return "Linux";
        case kOSX:
            return "OSX";
        case kIOS:
            return "iOS";
        case kCygwin:
            return "Cygwin";
        case kWindows:
            return "Windows";
        default:
            return "Unknown";
    }
}


std::string platformVersionString()
{
#if __linux__ || __APPLE__ || __CYGWIN__
    struct utsname n;
    int ret = ::uname(&n);
    if (ret != 0)
        throw eRuntimeError(strerror(errno));
    return std::string(n.release);
#elif __MINGW32__
    OSVERSIONINFOEX osvi;
    ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
    GetVersionEx((OSVERSIONINFO*)(&osvi));
    std::ostringstream out;
    out << osvi.dwMajorVersion << "." << osvi.dwMinorVersion << "." << osvi.dwBuildNumber
        << "_"
        << osvi.wServicePackMajor << "." << osvi.wServicePackMinor
        << "_"
        << osvi.wSuiteMask << "." << (int)osvi.wProductType;
    return out.str();
#else
#error What platform are you on!?
#endif
}


int getMachineBitness()
{
    return (int)(sizeof(void*) * 8);
}


}    // namespace rho
