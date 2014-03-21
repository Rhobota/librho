#ifndef __rho_types_h__
#define __rho_types_h__


#include <stdint.h>   // posix header file

#include <string>


namespace rho
{


typedef int8_t   i8;
typedef uint8_t  u8;

typedef int16_t  i16;
typedef uint16_t u16;

typedef int32_t  i32;
typedef uint32_t u32;

typedef int64_t  i64;
typedef uint64_t u64;

typedef float    f32;
typedef double   f64;


enum nPlatform
{
    kLinux,
    kOSX,
    kIOS,
    kCygwin,
    kWindows
};


nPlatform getCurrPlatform();

std::string platformEnumToString(nPlatform plat);

std::string platformVersionString();

int getMachineBitness();


}  // namespace rho


#endif    // __rho_types_h__
