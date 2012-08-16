#include <rho/img/tImage.h>


namespace rho
{
namespace img
{


tImage::tImage()
    : buf(NULL),
      bufSize(0),
      bufUsed(0),
      width(0),
      height(0),
      format(kUnspecified)
{
}

tImage::tImage(u8* buf, u32 bufSize)
    : buf(buf),
      bufSize(bufSize),
      bufUsed(0),
      width(0),
      height(0),
      format(kUnspecified)
{
}

tImage::~tImage()
{
    if (buf)
        delete [] buf;
    buf = NULL;
    bufSize = 0;
    bufUsed = 0;
    width = 0;
    height = 0;
    format = kUnspecified;
}


}     // namespace img
}     // namespace rho
