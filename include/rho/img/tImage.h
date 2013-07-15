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
        tImage(u32 bufSize);               // <-- allocates a buffer of that size
        tImage(std::string filepath,
               nImageFormat format);
        ~tImage();

        void copyTo(tImage* other) const;  // <-- copies must be explicit
        void convertToFormat(nImageFormat format, tImage* dest) const;

        void setBufSize(u32 bufSize);      // <-- allocates a buffer of that size
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

        void verticalFlip();
        void horizontalFlip();

        void crop (geo::tRect rect,       tImage* dest)  const;
        void scale(u32 width, u32 height, tImage* dest)  const;

    public:

        struct tRow
        {
            u8* m_rowbuf;
            u32 m_bpp;
            u8* operator[] (size_t index) { return m_rowbuf+index*m_bpp; }
        };

        tRow operator[] (size_t index);

    public:

        // iPackable interface
        void pack(iWritable* out) const;
        void unpack(iReadable* in);

    private:

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
