#ifndef __rho_img_tImage_h__
#define __rho_img_tImage_h__


#include <rho/img/nImageFormat.h>
#include <rho/bNonCopyable.h>
#include <cstdlib>


namespace rho
{
namespace img
{


class tImage : public bNonCopyable
{
    public:

        tImage();
        tImage(u8* buf, u32 bufSize);
        ~tImage();

    public:

        u8*          buf;
        u32          bufSize;
        u32          bufUsed;
        u32          width;
        u32          height;
        nImageFormat format;
};


}    // namespace img
}    // namespace rho


#endif   // __rho_img_tImage_h__
