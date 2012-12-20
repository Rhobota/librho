#include <rho/img/tImage.h>
#include <rho/eRho.h>

#include <algorithm>
#include <cmath>

#include "stb_image.h"


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

tImage::tImage(std::string filepath, nImageFormat format)
    : m_buf(NULL),
      m_bufSize(0),
      m_bufUsed(0),
      m_width(0),
      m_height(0),
      m_format(kUnspecified)
{
    int width, height, numComponents, req;
    nImageFormat readFormat;
    switch (format)
    {
        case kRGB16:
        case kRGB24:
        case kYUYV:
            req = 3;
            readFormat = kRGB24;
            break;
        case kRGBA:
        case kBGRA:
            req = 4;
            readFormat = kRGBA;
            break;
        case kGrey:
            req = 1;
            readFormat = kGrey;
            break;
        default:
            throw eInvalidArgument("The format you specified is not supported.");
    }
    u8* buf = stbi_load(filepath.c_str(), &width, &height, &numComponents, req);
    if (buf == NULL)
        throw eInvalidArgument("The filepath specified contains a bad image.");
    u32 bufSize = width * height * req;
    setBufSize(bufSize);
    for (u32 i = 0; i < bufSize; i++)
        m_buf[i] = buf[i];
    stbi_image_free(buf);
    setBufUsed(bufSize);
    setWidth(width);
    setHeight(height);
    setFormat(readFormat);
    tImage image;
    convertToFormat(format, &image);
    image.copyTo(this);
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

void tImage::copyTo(tImage* other) const
{
    other->setBufSize(m_bufSize);
    other->setBufUsed(m_bufUsed);
    other->setWidth(m_width);
    other->setHeight(m_height);
    other->setFormat(m_format);
    for (u32 i = 0; i < m_bufUsed; i++)
        other->m_buf[i] = m_buf[i];
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

const u8* tImage::buf() const
{
    return m_buf;
}

u32 tImage::bufSize() const
{
    return m_bufSize;
}

u32 tImage::bufUsed() const
{
    return m_bufUsed;
}

u32 tImage::width() const
{
    return m_width;
}

u32 tImage::height() const
{
    return m_height;
}

nImageFormat tImage::format() const
{
    return m_format;
}

static
void s_verticalFlip(tImage* image)
{
    u8* buf     = image->buf();
    u32 bufUsed = image->bufUsed();
    u32 width   = image->width();
    u32 height  = image->height();

    if (width == 0 || height == 0)
        return;

    if (bufUsed % (width * height))
        throw eLogicError("Something is wack with the given image.");

    u32 bpp = bufUsed / (width * height);   // bytes-per-pixel

    for (u32 h = 0; h < height; h++)
    {
        u8* row = buf + (h * width * bpp);
        for (u32 l=0, r=width-1; l < width/2; l++, r--)
        {
            u8* lp = row + (l * bpp);
            u8* rp = row + (r * bpp);
            for (u32 i = 0; i < bpp; i++)
                std::swap(lp[i], rp[i]);
        }
    }
}

static
void s_crop(const tImage* image, geo::tRect rect, tImage* dest)
{
    int x = rect.x;
    int y = rect.y;
    int width = rect.width;
    int height = rect.height;

    if (x < 0 || (u32)x > image->width())
        x = 0;

    if (y < 0 || (u32)y > image->height())
        y = 0;

    if (width < 0)
        width = 0;

    if (height < 0)
        height = 0;

    if ((u32)(x + width) > image->width())
        width = image->width() - x;

    if ((u32)(y + height) > image->height())
        height = image->height() - y;

    dest->setBufUsed(0);
    dest->setWidth(width);
    dest->setHeight(height);
    dest->setFormat(image->format());

    if (image->width() == 0 || image->height() == 0)
        return;

    if (image->bufUsed() % (image->width() * image->height()))
        throw eLogicError("Something is wack with the given image.");

    int bpp = image->bufUsed() / (image->width() * image->height());

    dest->setBufUsed(width * height * bpp);

    if (dest->bufSize() < dest->bufUsed())
        dest->setBufSize(dest->bufUsed());

    const u8* sbuf = image->buf() + (y*image->width() + x) * bpp;
    u8* dbuf = dest->buf();

    for (int j = 0; j < height; j++)
    {
        for (int i = 0; i < width; i++)
        {
            for (int k = 0; k < bpp; k++)
            {
                *dbuf++ = *sbuf++;
            }
        }
        sbuf += (image->width() - width) * bpp;
    }
}

static
void s_scale(const tImage* from, double scaleFactor, tImage* to)
{
    u32 width  = from->width() * scaleFactor;
    u32 height = from->height() * scaleFactor;

    to->setBufUsed(0);
    to->setWidth(width);
    to->setHeight(height);
    to->setFormat(from->format());

    if (from->width() == 0 || from->height() == 0)
        return;

    if (from->bufUsed() % (from->width() * from->height()))
        throw eLogicError("Something is wack with the given image.");

    u32 bpp = from->bufUsed() / (from->width() * from->height());

    to->setBufUsed(width*height*bpp);

    if (to->bufSize() < to->bufUsed())
        to->setBufSize(to->bufUsed());

    u32 stride = floor(1.0 / scaleFactor);
    u32 xstride = stride * bpp;
    u32 ystride = (stride * from->width() * bpp) - (width * xstride);

    const u8* fbuf = from->buf();
    u8* tbuf = to->buf();

    for (u32 y = 0; y < height; y++)
    {
        for (u32 x = 0; x < width; x++)
        {
            for (u32 k = 0; k < bpp; k++)
            {
                *tbuf++ = fbuf[k];
            }
            fbuf += xstride;
        }
        fbuf += ystride;
    }
}

void tImage::verticalFlip()
{
    s_verticalFlip(this);
}

void tImage::crop(geo::tRect rect, tImage* dest)  const
{
    s_crop(this, rect, dest);
}

void tImage::scale(double scaleFactor, tImage* dest)  const
{
    s_scale(this, scaleFactor, dest);
}

void tImage::convertToFormat(nImageFormat format, tImage* dest) const
{
    dest->setBufSize(1024);
    while (true)
    {
        try
        {
            int bufUsed = colorspace_conversion(m_format, format,
                    m_buf, m_bufUsed,
                    dest->buf(), dest->bufSize());
            dest->setBufUsed(bufUsed);
            dest->setWidth(m_width);
            dest->setHeight(m_height);
            dest->setFormat(format);
            break;
        }
        catch (eBufferOverflow& e)
        {
            u32 bufSize = dest->bufSize();
            bufSize *= 2;
            dest->setBufSize(bufSize);
        }
    }
}

void tImage::pack(iOutputStream* out) const
{
    rho::pack(out, m_bufUsed);
    for (u32 i = 0; i < m_bufUsed; i++)
        rho::pack(out, m_buf[i]);
    rho::pack(out, m_width);
    rho::pack(out, m_height);
    rho::pack(out, (u32)m_format);
}

void tImage::unpack(iInputStream* in)
{
    if (m_buf)
        delete [] m_buf;
    rho::unpack(in, m_bufUsed);
    m_bufSize = m_bufUsed;
    m_buf = new u8[m_bufSize];
    for (u32 i = 0; i < m_bufUsed; i++)
        rho::unpack(in, m_buf[i]);
    rho::unpack(in, m_width);
    rho::unpack(in, m_height);
    u32 format; rho::unpack(in, format);
    m_format = (nImageFormat)format;
}


}     // namespace img
}     // namespace rho
