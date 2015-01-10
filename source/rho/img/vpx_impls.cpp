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
}

void tVpxImageEncoder::encodeImage(const tImage& image)
{
}


}  // namespace img
}  // namespace rho
