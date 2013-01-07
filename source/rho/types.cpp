#include <rho/types.h>


namespace rho
{


nPlatform getCurrPlatform()
{
    #if __linux__
    return kLinux;
    #elif __APPLE__
    return kOSX;
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
