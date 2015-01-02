#ifndef __rho_img_tImageAsyncReadable_h__
#define __rho_img_tImageAsyncReadable_h__


#include <rho/iAsyncReadable.h>
#include <rho/img/tImage.h>


namespace rho
{
namespace img
{


class iAsyncReadableImageObserver
{
    public:

        virtual void gotImage(tImage* image) = 0;

        virtual ~iAsyncReadableImageObserver() { }
};


class tImageAsyncReadable : public iAsyncReadable, public bNonCopyable
{
    public:

        tImageAsyncReadable(iAsyncReadableImageObserver* observer);

        void takeInput(const u8* buffer, i32 length);
        void endStream();

    private:

        iAsyncReadableImageObserver* m_observer;

        tImage m_image;

        enum nStage {
            kStageBufUsed,
            kStageBufData,
            kStageWidth,
            kStageHeight,
            kStageFormat
        };

        nStage m_stage;

        u8 m_buf[4];
        u32 m_bufPos;
};


}  // namespace img
}  // namespace rho


#endif  // __rho_img_tImageAsyncReadable_h__
