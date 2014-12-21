#ifndef __rho_img_tImage_h__
#define __rho_img_tImage_h__


#include <rho/ppcheck.h>
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
         * Applies a median filter to the image with the specified window
         * size.
         */
        void medianFilter(tImage* dest,
                          u32 windowWidth=5,
                          u32 windowHeight=5) const;

        /**
         * Finds edges using the Sobel operator, as described by:
         *     http://en.wikipedia.org/wiki/Sobel_operator
         *
         * The 'clipAtValue' parameter is used to specify the
         * maximum gradient value which can be represented in
         * the returned image. Any gradient value more than it
         * will be clipped to that max value. That is, all values
         * of the gradients will be clipped to be in [0, clipAtValue].
         * After clipping, the range will be normalized to [0, 255]
         * so that the full range of the returned image is utilized.
         */
        void sobel(tImage* dest, u32 clipAtValue=255) const;

        /**
         * A struct used in the houghCircles() method below.
         */
        struct tHoughCircle
        {
            tHoughCircle(u32 x, u32 y, u32 r, u32 b, u32 v) : x(x), y(y), r(r), b(b), v(v) { }

            u32 x;     // the x-coord of this circle
            u32 y;     // the y-coord of this circle
            u32 r;     // the radius of this circle
            u32 b;     // the index of the channel this circle was found on
            u32 v;     // the number of "votes" for this circle
        };

        /**
         * The Hough Transform for Circles.
         *
         * The search space is defined by x_min, x_max, x_step, y_min,
         * y_max, y_step, r_min, r_max, and r_step. So, be careful that
         * you don't define a search space that is too big, or this method
         * will run for a very long time.
         *
         * 'allRadiiAccumulator' is an out-param. On return, it will have dimensions:
         *     ((y_max - y_min) / y_step + 1, (x_max - x_min) / x_step + 1)
         *
         * Each cell in 'allRadiiAccumulator' will contain the number of "votes"
         * that center received for ALL the radii tested on ALL channels of the image.
         *
         * An array of tHoughCircle objects will be returned, sorted from "most likely
         * a circle" to "least likely a circle". A circle is only put into this array
         * if it gets greater than or equal "votes" than 'voteThresh'.
         */
        std::vector<tHoughCircle> houghCircles(std::vector< std::vector<u32> >& allRadiiAccumulator,
                                               u32 x_min, u32 x_max, u32 x_step,
                                               u32 y_min, u32 y_max, u32 y_step,
                                               u32 r_min, u32 r_max, u32 r_step,
                                               u32 voteThresh) const;

    public:

        struct tRow
        {
            u8* m_rowbuf;
            u32 m_bpp;

            u8*       operator[] (size_t index)       { return m_rowbuf+index*m_bpp; }
            const u8* operator[] (size_t index) const { return m_rowbuf+index*m_bpp; }
        };

        tRow       operator[] (size_t index)
        {
            tRow row;
            u32 bpp = getBPP(m_format);
            row.m_rowbuf = m_buf + index*m_width*bpp;
            row.m_bpp = bpp;
            return row;
        }

        const tRow operator[] (size_t index) const
        {
            tRow row;
            u32 bpp = getBPP(m_format);
            row.m_rowbuf = m_buf + index*m_width*bpp;
            row.m_bpp = bpp;
            return row;
        }

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
