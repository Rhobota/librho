#if __linux__
#pragma GCC optimize 3
#endif

#include <rho/img/tImage.h>
#include <rho/eRho.h>

#include <algorithm>
#include <cmath>
#include <sstream>

#include "stb_image.h"
#include "lodepng.h"


namespace rho
{
namespace img
{


tImage::tImage()
    : m_buf(new u8[1024]),
      m_bufSize(1024),
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
    i32 width, height, numComponents, req;
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
    {
        std::ostringstream out;
        out << "Cannot load image (" << stbi_failure_reason() << "): " << filepath;
        throw eInvalidArgument(out.str());
    }
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
    if (this == other)
        return;
    other->setBufSize(m_bufSize);
    other->setBufUsed(m_bufUsed);
    other->setWidth(m_width);
    other->setHeight(m_height);
    other->setFormat(m_format);
    for (u32 i = 0; i < m_bufUsed; i++)
        other->m_buf[i] = m_buf[i];
}

void tImage::saveToFile(std::string filepath) const
{
    LodePNGColorType saveFormat;
    nImageFormat needFormat;
    switch (m_format)
    {
        case kRGB16:
        case kRGB24:
        case kYUYV:
            saveFormat = LCT_RGB;
            needFormat = kRGB24;
            break;
        case kRGBA:
        case kBGRA:
            saveFormat = LCT_RGBA;
            needFormat = kRGBA;
            break;
        case kGrey:
            saveFormat = LCT_GREY;
            needFormat = kGrey;
            break;
        default:
            throw eLogicError("The format of this image is unknown and cannot be saved to a file.");
    }

    unsigned err;

    if (m_format == needFormat)
    {
        err = lodepng_encode_file(filepath.c_str(), m_buf, m_width, m_height,
                                  saveFormat, 8);
    }
    else
    {
        tImage image;
        convertToFormat(needFormat, &image);
        err = lodepng_encode_file(filepath.c_str(), image.buf(), image.width(), image.height(),
                                  saveFormat, 8);
    }

    if (err != 0)
    {
        std::ostringstream out;
        out << "Cannot save image (" << lodepng_error_text(err) << "): " << filepath;
        throw eLogicError(out.str());
    }
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

    u8* row = buf;
    for (u32 h = 0; h < height; h++)
    {
        u8* lp = row;
        u8* rp = row + bpp*(width-1);
        while (lp < rp)
        {
            for (u32 i = 0; i < bpp; i++)
                std::swap(lp[i], rp[i]);
            lp += bpp;
            rp -= bpp;
        }
        row += bpp * width;
    }
}

static
void s_horizontalFlip(tImage* image)
{
    u8* buf     = image->buf();
    u32 bufUsed = image->bufUsed();
    u32 width   = image->width();
    u32 height  = image->height();

    if (width == 0 || height == 0)
        return;

    if (bufUsed % (width * height))
        throw eLogicError("Something is wack with the given image.");

    u32 bpr = bufUsed / height;              // bytes-per-row

    for (u32 t = 0, b = height-1; t < b; t++, b--)
    {
        u32 tOff = t * bpr;
        u32 bOff = b * bpr;
        for (u32 i = 0; i < bpr; i++)
            std::swap(buf[tOff++], buf[bOff++]);
    }
}

static
void s_crop(const tImage* image, geo::tRect rect, tImage* dest)
{
    if (image == dest)
        throw eInvalidArgument("s_crop(): source and destination must be different objects");

    if (image->width() == 0 || image->height() == 0)
    {
        dest->setBufUsed(0);
        dest->setWidth(0);
        dest->setHeight(0);
        dest->setFormat(image->format());
        return;
    }

    if (image->bufUsed() % (image->width() * image->height()))
        throw eLogicError("Something is wack with the given image.");

    i32 bpp = image->bufUsed() / (image->width() * image->height());

    if (rect.x < 0.0)
        rect.x = 0.0;

    if (rect.y < 0.0)
        rect.y = 0.0;

    if (rect.x > image->width())
        rect.x = image->width();

    if (rect.y > image->height())
        rect.y = image->height();

    if (rect.width < 0.0)
        rect.width = 0.0;

    if (rect.height < 0.0)
        rect.height = 0.0;

    if ((rect.x + rect.width) > image->width())
        rect.width = image->width() - rect.x;

    if ((rect.y + rect.height) > image->height())
        rect.height = image->height() - rect.y;

    u32 x = (u32) rect.x;
    u32 y = (u32) rect.y;
    u32 width = (u32) rect.width;
    u32 height = (u32) rect.height;

    dest->setBufUsed(width * height * bpp);
    dest->setWidth(width);
    dest->setHeight(height);
    dest->setFormat(image->format());

    if (dest->bufSize() < dest->bufUsed())
        dest->setBufSize(dest->bufUsed());

    const u8* sbuf = image->buf() + (y*image->width() + x) * bpp;
    u8* dbuf = dest->buf();

    for (u32 j = 0; j < height; j++)
    {
        for (u32 i = 0; i < width; i++)
        {
            for (i32 k = 0; k < bpp; k++)
            {
                *dbuf++ = *sbuf++;
            }
        }
        sbuf += (image->width() - width) * bpp;
    }
}

static
u8 s_avg(const tImage* image, geo::tRect rect, u32 pixOffset, u32 bpp)   // helper for s_scale()
{
    u32 x = (u32) rect.x;
    u32 y = (u32) rect.y;
    u32 xend = (u32) std::ceil(rect.x + rect.width);
    u32 yend = (u32) std::ceil(rect.y + rect.height);

    if (xend > image->width())
        xend = image->width();
    if (yend > image->height())
        yend = image->height();

    const u8* buf = image->buf() + (y*image->width() + x) * bpp + pixOffset;

    u32 numpix = (xend-x) * (yend-y);

    u8 avg = 0;
    u32 count = 0;
    for (u32 r = y; r < yend; r++)
    {
        for (u32 c = x; c < xend; c++)
        {
            count += *buf;
            while (count >= numpix)
            {
                avg++;
                count -= numpix;
            }
            buf += bpp;
        }
        buf += (image->width() - xend+x) * bpp;
    }

    return avg;
}

static
void s_scale(const tImage* from, u32 width, u32 height, tImage* to)
{
    if (from == to)
        throw eInvalidArgument("s_scale(): source and destination must be different objects");

    if (from->width() == 0 || from->height() == 0)
        throw eInvalidArgument("s_scale(): cannot scale an image of no width or no height");

    if (from->bufUsed() % (from->width() * from->height()))
        throw eLogicError("Something is wack with the given image.");

    u32 bpp = from->bufUsed() / (from->width() * from->height());

    to->setBufUsed(width*height*bpp);
    to->setWidth(width);
    to->setHeight(height);
    to->setFormat(from->format());

    if (to->bufSize() < to->bufUsed())
        to->setBufSize(to->bufUsed());

    u8* tbuf = to->buf();

    f64 wStretch = ((f64)from->width()) / to->width();
    f64 hStretch = ((f64)from->height()) / to->height();

    geo::tRect rect(0.0, 0.0, wStretch, hStretch);

    for (u32 h = 0; h < height; h++)
    {
        rect.x = 0.0;
        for (u32 w = 0; w < width; w++)
        {
            for (u32 i = 0; i < bpp; i++)
            {
                *tbuf++ = s_avg(from, rect, i, bpp);
            }
            rect.x += wStretch;
        }
        rect.y += hStretch;
    }
}

void tImage::verticalFlip()
{
    s_verticalFlip(this);
}

void tImage::horizontalFlip()
{
    s_horizontalFlip(this);
}

void tImage::crop(geo::tRect rect, tImage* dest)  const
{
    s_crop(this, rect, dest);
}

void tImage::scale(u32 width, u32 height, tImage* dest)  const
{
    s_scale(this, width, height, dest);
}

static
void s_colorImageAtPoint(tImage* image, i32 npix, i32 x, i32 y, const u8* colorBuf, double percentage)
{
    if (x < 0 || x >= (i32)image->width())
        return;
    if (y < 0 || y >= (i32)image->height())
        return;

    u8* buf = image->buf() + npix*(y*image->width()+x);
    for (i32 i = 0; i < npix; i++)
    {
        u32 val = (u32) (buf[i] + colorBuf[i]*percentage);
        buf[i] = (u8) std::min((u32)255, val);
    }
}

static
void s_colorImageAtPoint(tImage* image, i32 npix, double x, double y, const u8* colorBuf)
{
    i32 xFloor = (i32) floor(x);
    i32 yFloor = (i32) floor(y);

    i32 xCeil = (i32) ceil(x);
    i32 yCeil = (i32) ceil(y);

    double leftRatio = xCeil == x ? 1.0 :  xCeil - x;
    double topRatio = yCeil == y ? 1.0 :  yCeil - y;

    // Left-top pixel
    s_colorImageAtPoint(image, npix, xFloor, yFloor, colorBuf, leftRatio*topRatio);

    // Right-top pixel
    s_colorImageAtPoint(image, npix, xCeil, yFloor, colorBuf, (1.0-leftRatio)*topRatio);

    // Right-bottom pixel
    s_colorImageAtPoint(image, npix, xCeil, yCeil, colorBuf, (1.0-leftRatio)*(1.0-topRatio));

    // Left-bottom pixel
    s_colorImageAtPoint(image, npix, xFloor, yCeil, colorBuf, leftRatio*(1.0-topRatio));
}

void tImage::rotate(double angleDegrees, tImage* dest) const
{
    // Stuff that will be needed below.
    i32 npix = bufUsed() / (width()*height());
    double angleRad = (geo::kPI / 180.0) * angleDegrees;

    // Calculate how big the canvas needs to be.
    double diagAng = atan(((double)height())/width());
    double diagLen = hypot(width()/2.0, height()/2.0);
    double newWidth  = std::max(fabs(cos(angleRad+diagAng)), fabs(cos(angleRad-diagAng))) * diagLen * 2;
    double newHeight = std::max(fabs(sin(angleRad+diagAng)), fabs(sin(angleRad-diagAng))) * diagLen * 2;
    double halfWidthGain = (newWidth-width())/2.0;
    double halfHeightGain = (newHeight-height())/2.0;

    // Create a black canvas for drawing onto.
    tImage& canvas = *dest;
    canvas.setFormat(format());
    canvas.setWidth((u32)newWidth);
    canvas.setHeight((u32)newHeight);
    canvas.setBufSize(npix*canvas.width()*canvas.height());
    canvas.setBufUsed(canvas.bufSize());
    for (u32 i = 0; i < canvas.bufUsed(); i++) canvas.buf()[i] = 0;

    // Create the rotation matrix's cosine and sine elements.
    double c = std::cos(angleRad);
    double s = std::sin(angleRad);

    // For each pixel in the image, draw it onto the canvas at its rotated position.
    double originX = width()/2.0;
    double originY = height()/2.0;
    for (i32 y = 0; y < (i32)height(); y++)
    {
        for (i32 x = 0; x < (i32)width(); x++)
        {
            const u8* imageBuf = buf() + npix*(y*width()+x);

            double xShift = x-originX;
            double yShift = y-originY;

            double xRotShift = xShift*c + yShift*s;
            double yRotShift = -xShift*s + yShift*c;

            double xRot = xRotShift+originX;
            double yRot = yRotShift+originY;

            xRot += halfWidthGain;
            yRot += halfHeightGain;

            s_colorImageAtPoint(&canvas, npix, xRot, yRot, imageBuf);
        }
    }
}

void tImage::convertToFormat(nImageFormat format, tImage* dest) const
{
    if (this == dest)
        throw eInvalidArgument("convertToFormat(): source and destination must be different objects");

    dest->setBufSize(1024);
    while (true)
    {
        try
        {
            i32 bufUsed = colorspace_conversion(m_format, format,
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

static
void s_adaptiveThreshold(tImage* image, u32 s, u32 t, u32 b)
{
    u32 g = b;
    std::vector<u32> prev_g(image->width(), b);

    u8* buf = image->buf();

    for (u32 h = 0; h < image->height(); h++)
    {
        for (u32 w = 0; w < image->width(); w++)
        {
            g -= g/s;
            g += *buf;
            u32 hh = (g + prev_g[w]) / 2;
            if (*buf < hh/s*(100-t)/100)
                *buf = 0;
            else
                *buf = 255;
            prev_g[w] = g;
            buf++;
        }
        buf += image->width()-1;
        h++;
        if (h >= image->height())
            break;
        for (u32 w = image->width()-1; (i32)w >= 0; w--)
        {
            g -= g/s;
            g += *buf;
            u32 hh = (g + prev_g[w]) / 2;
            if (*buf < hh/s*(100-t)/100)
                *buf = 0;
            else
                *buf = 255;
            prev_g[w] = g;
            buf--;
        }
        buf += image->width()+1;
    }
}

void tImage::adaptiveThreshold(tImage* dest,
                               i32 s,
                               i32 t,
                               i32 b) const
{
    if (s == -1)
        s = width() / 8;
    if (s < 0)
        throw eInvalidArgument("s must be >= 0 (or -1 to indicate the default value)");
    if (t < 0)
        throw eInvalidArgument("t must be >= 0");
    if (b < 0)
        throw eInvalidArgument("b must be >= 0");

    convertToFormat(kGrey, dest);
    s_adaptiveThreshold(dest, (u32)s, (u32)t, (u32)b);
}

static
void s_sobel(const img::tImage* orig, img::tImage* edges)
{
    u32 bpp = img::getBPP(orig->format());

    edges->setFormat(orig->format());
    edges->setWidth(orig->width() - 2);
    edges->setHeight(orig->height() - 2);
    edges->setBufSize(edges->width() * edges->height() * bpp);
    edges->setBufUsed(edges->bufSize());

    for (u32 row = 0; row < edges->height(); row++)
    {
        for (u32 col = 0; col < edges->width(); col++)
        {
            for (u32 k = 0; k < bpp; k++)
            {
                u32 a = (*orig)[row+0][col+0][k];
                u32 b = (*orig)[row+0][col+1][k];
                u32 c = (*orig)[row+0][col+2][k];
                u32 d = (*orig)[row+1][col+0][k];
                //u32 e = (*orig)[row+1][col+1][k];
                u32 f = (*orig)[row+1][col+2][k];
                u32 g = (*orig)[row+2][col+0][k];
                u32 h = (*orig)[row+2][col+1][k];
                u32 i = (*orig)[row+2][col+2][k];
                u32 gx = 1*a + 2*d + 1*g - 1*c - 2*f - 1*i;
                u32 gy = 1*a + 2*b + 1*c - 1*g - 2*h - 1*i;
                u32 grad = (u32) std::sqrt(gx*gx + gy*gy);
                (*edges)[row][col][k] = ((grad > 255) ? 255 : grad);
            }
        }
    }
}

void tImage::sobel(tImage* dest) const
{
    s_sobel(this, dest);
}

u8* tImage::tRow::operator[] (size_t index)
{
    return m_rowbuf+index*m_bpp;
}

const u8* tImage::tRow::operator[] (size_t index) const
{
    return m_rowbuf+index*m_bpp;
}

tImage::tRow tImage::operator[] (size_t index)
{
    tRow row;
    u32 bpp = getBPP(m_format);
    row.m_rowbuf = m_buf + index*m_width*bpp;
    row.m_bpp = bpp;
    return row;
}

const tImage::tRow tImage::operator[] (size_t index) const
{
    tRow row;
    u32 bpp = getBPP(m_format);
    row.m_rowbuf = m_buf + index*m_width*bpp;
    row.m_bpp = bpp;
    return row;
}

void tImage::pack(iWritable* out) const
{
    rho::pack(out, m_bufUsed);
    for (u32 i = 0; i < m_bufUsed; i++)
        rho::pack(out, m_buf[i]);
    rho::pack(out, m_width);
    rho::pack(out, m_height);
    rho::pack(out, (u32)m_format);
}

void tImage::unpack(iReadable* in)
{
    try
    {
        if (m_buf)
        {
            delete [] m_buf;
            m_buf = NULL;
        }
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
    catch (std::exception& e)
    {
        setBufSize(1024);
        setBufUsed(0);
        setWidth(0);
        setHeight(0);
        setFormat(kUnspecified);
        throw;
    }
}


}     // namespace img
}     // namespace rho
