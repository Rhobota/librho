#ifndef __rho_img_nImageFormat_h__
#define __rho_img_nImageFormat_h__


#include <rho/types.h>


namespace rho
{
namespace img
{


enum nImageFormat
{
    kRGB16          = 0,
    kRGB24          = 1,
    kRGBA           = 2,
    kYUYV           = 3,

    kMaxImageFormat = 4,

    kUnspecified    = 98,
    kUnknown        = 99
};


int colorspace_conversion(nImageFormat from, nImageFormat to,
                          u8* source, int sourceSize,
                          u8* dest, int destSize);


}  // namespace img
}  // namespace rho


#endif    // __rho_img_nImageFormat_h__
