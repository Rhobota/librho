#ifndef __rho_img_tImage_h__
#define __rho_img_tImage_h__


#include <rho/bNonCopyable.h>
#include <rho/geo/tRect.h>
#include <rho/img/nImageFormat.h>
#include <rho/iPackable.h>

#include <cstdlib>
#include <string>


namespace rho
{
namespace img
{


class tImage : public bNonCopyable, public iPackable
{
    public:

        tImage();
        tImage(u32 bufSize);               // allocates a buffer of that size

        tImage(std::string filepath, nImageFormat format);

        ~tImage();

        void copyTo(tImage* other) const;  // copies should be explicit

        void setBufSize(u32 bufSize);      // allocates a buffer of that size
        void setBufUsed(u32 bufUsed);
        void setWidth(u32 width);
        void setHeight(u32 height);
        void setFormat(nImageFormat format);

        u8*          buf();
        const u8*    buf()      const;
        u32          bufSize()  const;
        u32          bufUsed()  const;
        u32          width()    const;
        u32          height()   const;
        nImageFormat format()   const;

        void convertToFormat(nImageFormat format, tImage* dest) const;

        void verticalFlip();

        void crop (geo::tRect rect,    tImage* dest)  const;
        void scale(double scaleFactor, tImage* dest)  const;

        template <int N>
        struct tPixel { u8 data[N]; };

        template <int N>
        tPixel<N> getpix(u32 x, u32 y) const;

        template <int N>
        void setpix(u32 x, u32 y, tPixel<N> pixel);

        // iPackable interface
        void pack(iOutputStream* out) const;
        void unpack(iInputStream* in);

    public:

        u8*          m_buf;
        u32          m_bufSize;
        u32          m_bufUsed;
        u32          m_width;
        u32          m_height;
        nImageFormat m_format;
};


template <int N>
tImage::tPixel<N> tImage::getpix(u32 x, u32 y) const
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
