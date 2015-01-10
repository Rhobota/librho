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

        iAsyncReadableImageObserver* m_observer;
};


}  // namespace img
}  // namespace rho


#endif  // __rho_img_tVpxImageAsyncReadable_h__
