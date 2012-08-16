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
        tImage(u32 bufSize);            // allocates a buffer of that size
        ~tImage();

        void setBufSize(u32 bufSize);   // allocates a buffer of that size
        void setBufUsed(u32 bufUsed);
        void setWidth(u32 width);
        void setHeight(u32 height);
        void setFormat(nImageFormat format);

        u8*          buf();
        u32          bufSize();
        u32          bufUsed();
        u32          width();
        u32          height();
        nImageFormat format();

        template <int N>
        struct pix { u8 p[N]; };

        template <int N>
        pix<N> getpix(u32 x, u32 y);

        template <int N>
        void setpix(u32 x, u32 y, pix<N> p);

    public:

        u8*          m_buf;
        u32          m_bufSize;
        u32          m_bufUsed;
        u32          m_width;
        u32          m_height;
        nImageFormat m_format;
};


}    // namespace img
}    // namespace rho


#endif   // __rho_img_tImage_h__
