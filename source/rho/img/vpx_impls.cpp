#include <rho/img/tVpxImageEncoder.h>


namespace rho
{
namespace img
{


tVpxImageEncoder::tVpxImageEncoder(iWritable* writable,
                 u32 bitrate,
                 u32 fps,
                 u32 imageWidth,
                 u32 imageHeight)
    : m_writable(writable),
      m_bitrate(bitrate),
      m_fps(fps),
      m_width(imageWidth),
      m_height(imageHeight),
      m_tempImage()
{
    // TODO
}

void tVpxImageEncoder::encodeImage(const tImage& image)
{
    // TODO
}


tVpxImageAsyncReadable::tVpxImageAsyncReadable(iAsyncReadableImageObserver* observer)
    : m_observer(observer)
{
    // TODO
}

void tVpxImageAsyncReadable::takeInput(const u8* buffer, i32 length)
{
    // TODO
}

void tVpxImageAsyncReadable::endStream()
{
    // TODO
}


}  // namespace img
}  // namespace rho
