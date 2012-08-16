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
        struct tPixel { u8 data[N]; };

        template <int N>
        tPixel<N> getpix(u32 x, u32 y);

        template <int N>
        void setpix(u32 x, u32 y, tPixel<N> pixel);

    public:

        u8*          m_buf;
        u32          m_bufSize;
        u32          m_bufUsed;
        u32          m_width;
        u32          m_height;
        nImageFormat m_format;
};


template <int N>
tImage::tPixel<N> tImage::getpix(u32 x, u32 y)
{
    tImage::tPixel<N> pixel;
    for (int i = 0; i < N; i++)
        pixel.data[i] = m_buf[y*m_width*N + x*N + i];
    return pixel;
}

template <int N>
void tImage::setpix(u32 x, u32 y, tImage::tPixel<N> pixel)
{
    for (int i = 0; i < N; i++)
        m_buf[y*m_width*N + x*N + i] = pixel.data[i];
}


}    // namespace img
}    // namespace rho


#endif   // __rho_img_tImage_h__
