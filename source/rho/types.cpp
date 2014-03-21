#include <rho/types.h>
#include <rho/eRho.h>

#ifdef __APPLE__
#include "TargetConditionals.h"
#endif

#include <errno.h>
#include <string.h>
#include <sys/utsname.h>


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
    struct utsname n;
    int ret = ::uname(&n);
    if (ret != 0)
        throw eRuntimeError(strerror(errno));
    return std::string(n.release);
}


int getMachineBitness()
{
    return (sizeof(void*) * 8);
}


}    // namespace rho
