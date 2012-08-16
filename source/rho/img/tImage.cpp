#include <rho/img/tImage.h>


namespace rho
{
namespace img
{


tImage::tImage()
    : m_buf(NULL),
      m_bufSize(0),
      m_bufUsed(0),
      m_width(0),
      m_height(0),
      m_format(kUnspecified)
{
}

tImage::tImage(u32 bufSize)
    : m_buf(new u8[bufSize]),
      m_bufSize(bufSize),
      m_bufUsed(0),
      m_width(0),
      m_height(0),
      m_format(kUnspecified)
{
}

tImage::~tImage()
{
    if (m_buf)
        delete [] m_buf;
    m_buf = NULL;
    m_bufSize = 0;
    m_bufUsed = 0;
    m_width = 0;
    m_height = 0;
    m_format = kUnspecified;
}

void tImage::setBufSize(u32 bufSize)
{
    if (m_buf)
        delete [] m_buf;
    m_buf = new u8[bufSize];
    m_bufSize = bufSize;
}

void tImage::setBufUsed(u32 bufUsed)
{
    m_bufUsed = bufUsed;
}

void tImage::setWidth(u32 width)
{
    m_width = width;
}

void tImage::setHeight(u32 height)
{
    m_height = height;
}

void tImage::setFormat(nImageFormat format)
{
    m_format = format;
}

u8* tImage::buf()
{
    return m_buf;
}

u32 tImage::bufSize()
{
    return m_bufSize;
}

u32 tImage::bufUsed()
{
    return m_bufUsed;
}

u32 tImage::width()
{
    return m_width;
}

u32 tImage::height()
{
    return m_height;
}

nImageFormat tImage::format()
{
    return m_format;
}


}     // namespace img
}     // namespace rho
