#ifndef __rho_img_nImageFormat_h__
#define __rho_img_nImageFormat_h__


#include <rho/ppcheck.h>
#include <rho/types.h>
#include <rho/eRho.h>


namespace rho
{
namespace img
{


enum nImageFormat
{
    kRGB16          = 0,      // RGB,  16 bits == 1 pixel
    kRGB24          = 1,      // RGB,  24 bits == 1 pixel
    kRGBA           = 2,      // RGBA, 32 bits == 1 pixel
    kBGRA           = 3,      // BGRA, 32 bits == 1 pixel
    kYUYV           = 4,      // YUYV, 32 bits == 2 pixels
    kGrey           = 5,      // Grey, 8 bits  == 1 pixel

    kMaxImageFormat = 6,

    kUnspecified    = 98,
    kUnknown        = 99
};


// Gets the number of bytes per pixel for the given image format.
u32 getBPP(nImageFormat format);


// Converts an image buffer. Returns the number of bytes used of 'dest'.
i32 colorspace_conversion(nImageFormat from, nImageFormat to,
                          u8* source, i32 sourceSize,
                          u8* dest, i32 destSize);


}  // namespace img
}  // namespace rho


#endif    // __rho_img_nImageFormat_h__
