#include <rho/img/tImageAsyncReadable.h>


namespace rho
{
namespace img
{


tImageAsyncReadable::tImageAsyncReadable(iAsyncReadableImageObserver* observer)
    : m_observer(observer)
{
}

void tImageAsyncReadable::takeInput(const u8* buffer, i32 length)
{
}

void tImageAsyncReadable::endStream()
{
}


}  // namespace img
}  // namespace rho
