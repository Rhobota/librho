#ifndef __rho_img_tImage_h__
#define __rho_img_tImage_h__


#include <rho/bNonCopyable.h>
#include <rho/geo/tRect.h>
#include <rho/img/nImageFormat.h>
#include <rho/iPackable.h>

#include <cstdlib>
#include <string>
#include <vector>


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

        void copyTo(tImage* other) const;  // <-- copies must be explicit with this method
        void convertToFormat(nImageFormat format, tImage* dest) const;

        void saveToFile(std::string filepath) const;

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

        void crop  (geo::tRect rect,       tImage* dest)  const;
        void scale (u32 width, u32 height, tImage* dest)  const;
        void rotate(double angleDegrees,   tImage* dest)  const;

        /**
         * Adaptive thresholds the receiving image and stores the binary,
         * grey-scale image result into 'dest'. Three parameters can be
         * modified:
         *     s: the number of trailing pixels that constitute the
         *        running average (if -1, s will be set to width()/8)
         *     t: the percent below the average that a pixel must be
         *        to be turned black (it is turned white otherwise)
         *     b: the starting running average value
         *
         * Algorithm first described by:
         *     http://www.cl.cam.ac.uk/techreports/UCAM-CL-TR-330.pdf
         */
        void adaptiveThreshold(tImage* dest,
                               i32 s = -1,
                               i32 t = 5,
                               i32 b = 127) const;

        /**
         * Finds edges using the Sobel operator.
         */
        void sobel(tImage* dest) const;

    public:

        struct tRow
        {
            u8* m_rowbuf;
            u32 m_bpp;

            u8*       operator[] (size_t index);
            const u8* operator[] (size_t index) const;
        };

        tRow       operator[] (size_t index);
        const tRow operator[] (size_t index) const;

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
