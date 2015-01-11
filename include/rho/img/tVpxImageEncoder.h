#ifndef __rho_img_tVpxImageEncoder_h__
#define __rho_img_tVpxImageEncoder_h__


#include <rho/bNonCopyable.h>
#include <rho/iPackable.h>
#include <rho/img/tImage.h>


namespace rho
{
namespace img
{


class tVpxImageEncoder : public bNonCopyable
{
    public:

        tVpxImageEncoder(iWritable* writable,   // <-- the stream to write encoded images to
                         u32 bitrate,           // <-- in kbits/second
                         u32 fps,               // <-- what to expect the frames-per-second to be
                         u32 imageWidth,        // <-- what to expect image widths to be
                         u32 imageHeight);      // <-- what to expect image heights to be

        ~tVpxImageEncoder();

        void encodeImage(const tImage& image,
                         bool flushWrites=true,
                         bool forceKeyframe=false);

        void signalEndOfStream();

    private:

        void m_convertImage(const tImage& image);

    private:

        iWritable* m_writable;
        u32 m_bitrate;
        u32 m_fps;
        u32 m_width;
        u32 m_height;
        i64 m_frameCount;

        void* m_codec;  // this is secretly a vpx_codec_ctx_t*
        void* m_vimage; // this is secretly a vpx_image_t*
};


}  // namespace img
}  // namespace rho


#endif // __rho_img_tVpxImageEncoder_h__
