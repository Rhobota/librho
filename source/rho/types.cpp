#include <rho/types.h>

#ifdef __APPLE__
#include "TargetConditionals.h"
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


int getMachineBitness()
{
    return (sizeof(void*) * 8);
}


}    // namespace rho
