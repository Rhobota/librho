#include <rho/img/tCanvas.h>

#include <cassert>


namespace rho
{
namespace img
{


static
void s_drawImage(const tImage* image, u32 x, u32 y, tImage* dest);


tCanvas::tCanvas(nImageFormat format, const u8 bgColor[], u32 bgColorSize)
{
    u32 bpp = getBPP(format);
    if (bpp != bgColorSize)
        throw eInvalidArgument("The bg color size does not match the image format's pixel size.");
    m_format = format;
    m_bgColor = new u8[bgColorSize];
    for (u32 i = 0; i < bgColorSize; i++)
        m_bgColor[i] = bgColor[i];
    m_bgColorSize = bgColorSize;
}

tCanvas::~tCanvas()
{
    reset();
    delete [] m_bgColor;
    m_bgColor = NULL;
    m_bgColorSize = 0;
}

void tCanvas::drawImage(const tImage* image, i32 x, i32 y)
{
    tImage* copy = new tImage();
    try
    {
        image->convertToFormat(m_format, copy);
    }
    catch (ebObject& e)
    {
        delete copy;
        throw;
    }
    m_images.push_back(copy);
    m_xs.push_back(x);
    m_ys.push_back(y);
    if (m_images.size() == 1)
        m_unionArea = geo::tRect(x, y, image->width(), image->height());
    else
        m_unionArea = m_unionArea.unionn(geo::tRect(x, y, image->width(), image->height()));
}

void tCanvas::expandToIncludePoint(i32 x, i32 y)
{
    if (m_images.size() == 0)
        m_unionArea = geo::tRect(x, y, 0.0, 0.0);
    else
        m_unionArea = m_unionArea.unionn(geo::tRect(x, y, 0.0, 0.0));
}

void tCanvas::reset()
{
    for (size_t i = 0; i < m_images.size(); i++)
        delete m_images[i];
    m_images.clear();
    m_xs.clear();
    m_ys.clear();
    m_unionArea = geo::tRect();
}

void tCanvas::genImage(tImage* dest)
{
    assert(m_images.size() == m_xs.size());
    assert(m_images.size() == m_ys.size());

    i32 xmin = (i32) std::floor(m_unionArea.x);
    i32 ymin = (i32) std::floor(m_unionArea.y);

    u32 width = (u32) std::ceil(m_unionArea.width);
    u32 height = (u32) std::ceil(m_unionArea.height);

    u32 bpp = getBPP(m_format);

    dest->setFormat(m_format);
    dest->setWidth(width);
    dest->setHeight(height);
    dest->setBufSize(width * height * bpp);
    dest->setBufUsed(width * height * bpp);
    u8* buf = dest->buf();
    for (u32 i = 0; i < dest->bufUsed(); i += bpp)
        for (u32 j = 0; j < bpp; j++)
            buf[i+j] = m_bgColor[j];

    for (size_t i = 0; i < m_images.size(); i++)
    {
        s_drawImage(m_images[i], m_xs[i]-xmin, m_ys[i]-ymin, dest);
    }
}


static
void s_drawImage(const tImage* image, u32 x, u32 y, tImage* dest)
{
    assert(x + image->width() <= dest->width());
    assert(y + image->height() <= dest->height());
    assert(image->format() == dest->format());

    u32 bpp = getBPP(image->format());

    const u8* sbuf = image->buf();
    u32 swidth = image->width();
    u32 sheight = image->height();

    u8* dbuf = dest->buf();
    u32 dwidth = dest->width();

    u32 drowwidth = dwidth*bpp;
    dbuf += y*drowwidth + x*bpp;

    for (u32 h = 0; h < sheight; h++)
    {
        u8* dbufrow = dbuf;
        for (u32 w = 0; w < swidth; w++)
            for (u32 b = 0; b < bpp; b++)
                *dbufrow++ = *sbuf++;
        dbuf += drowwidth;
    }
}


}   // namespace img
}   // namespace rho
