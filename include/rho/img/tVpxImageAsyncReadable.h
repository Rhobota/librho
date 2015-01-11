#ifndef __rho_img_tVpxImageAsyncReadable_h__
#define __rho_img_tVpxImageAsyncReadable_h__


#include <rho/bNonCopyable.h>
#include <rho/iPackable.h>
#include <rho/iAsyncReadable.h>
#include <rho/img/tImageAsyncReadable.h>


namespace rho
{
namespace img
{


class tVpxImageAsyncReadable : public iAsyncReadable, public bNonCopyable
{
    public:

        tVpxImageAsyncReadable(iAsyncReadableImageObserver* observer);

        void takeInput(const u8* buffer, i32 length);
        void endStream();

    private:

        void m_handleFrame();

    private:

        iAsyncReadableImageObserver* m_observer;

        void* m_codec;  // this is secretly a vpx_codec_ctx_t*

        tImage m_image;

        enum nStage {
            kStageFrameSize,
            kStageFrameData
        };

        nStage m_stage;

        u8 m_buf[4];
        u32 m_bufPos;

        u8* m_compressedBuf;
        u32 m_compressedBufSize;
        u32 m_compressedBufUsed;
};


}  // namespace img
}  // namespace rho


#endif  // __rho_img_tVpxImageAsyncReadable_h__
