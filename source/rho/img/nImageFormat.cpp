#include <rho/img/nImageFormat.h>
#include <rho/img/ebImg.h>


namespace rho
{
namespace img
{


static inline
u8 clip(double val)
{
    if (val > 255.0)
        return 255;
    else if (val < 0.0)
        return 0;
    return (u8)val;
}


static
int no_conversion(u8* source, int sourceSize,
                  u8* dest, int destSize)
{
    if (destSize < sourceSize)
        throw eBufferOverflow("You're doing it wrong.");
    for (int i = 0; i < sourceSize; i++)
        dest[i] = source[i];
    return sourceSize;
}


static
int yuyv_to_rgb24(u8* yuyv, int yuyvSize,
                  u8* rgb, int rgbSize)
{
    // yuyv images use four bytes to describe two pixels.
    // rbg images use three bytes to describe one pixel.

    if (yuyvSize % 4)
    {
        throw eColorspaceConversionError(
                "The yuyv image buffer has an incorrect size");
    }

    int numPixels = yuyvSize / 2;

    if (rgbSize < numPixels * 3)
    {
        throw eBufferOverflow(
                "The supplied rgb buffer cannot hold the image described by "
                "the supplied yuyv buffer.");
    }

    for(int i = 0; i < yuyvSize; i += 4)
    {
        u8 y0 = yuyv[i+0];
        u8 u  = yuyv[i+1];
        u8 y1 = yuyv[i+2];
        u8 v  = yuyv[i+3];

        /* standard: r0 = y0 + 1.402000*(v-128) */
        /* logitech: r0 = y0 + 1.370705*(v-128) */
        *rgb++ = clip(y0 + 1.402000*(v-128));

        /* standard: g0 = y0 - 0.344140*(u-128) - 0.714140*(v-128) */
        /* logitech: g0 = y0 - 0.337633*(u-128) - 0.698001*(v-128) */
        *rgb++ = clip(y0 - 0.344140*(u-128) - 0.714140*(v-128));

        /* standard: b0 = y0 + 1.772000*(u-128) */
        /* logitech: b0 = y0 + 1.732446*(u-128) */
        *rgb++ = clip(y0 + 1.772000*(u-128));

        /* standard: r1 = y1 + 1.402000*(v-128) */
        /* logitech: r1 = y1 + 1.370705*(v-128) */
        *rgb++ = clip(y1 + 1.402000*(v-128));

        /* standard: g1 = y1 - 0.344140*(u-128) - 0.714140*(v-128) */
        /* logitech: g1 = y1 - 0.337633*(u-128) - 0.698001*(v-128) */
        *rgb++ = clip(y1 - 0.344140*(u-128) - 0.714140*(v-128));

        /* standard: b1 = y1 + 1.772000*(u-128) */
        /* logitech: b1 = y1 + 1.732446*(u-128) */
        *rgb++ = clip(y1 + 1.772000*(u-128));
    }

    return numPixels * 3;
}


static
int rgb24_to_yuyv(u8* rgb, int rgbSize,
                  u8* yuyv, int yuyvSize)
{
    if (rgbSize % 3)
    {
        throw eColorspaceConversionError(
                "The yuyv image buffer has an incorrect size");
    }

    int numPixels = rgbSize / 3;

    if (numPixels % 2)
    {
        throw eColorspaceConversionError(
                "A yuyv image must have a multiple of two number of pixels.");
    }

    if (yuyvSize < numPixels * 2)
    {
        throw eBufferOverflow(
                "The supplied rgb buffer cannot hold the image described by "
                "the supplied yuyv buffer.");
    }

    for (int i = 0; i < rgbSize; i += 6)
    {
        u8 r0 = rgb[i+0];
        u8 g0 = rgb[i+1];
        u8 b0 = rgb[i+2];
        u8 r1 = rgb[i+3];
        u8 g1 = rgb[i+4];
        u8 b1 = rgb[i+5];

        *yuyv++ = clip(0.299 * (r0 - 128) + 0.587 * (g0 - 128) + 0.114 * (b0 - 128) + 128);
        *yuyv++ = clip(((- 0.147 * (r0 - 128) - 0.289 * (g0 - 128) + 0.436 * (b0 - 128) + 128) +
                       (- 0.147 * (r1 - 128) - 0.289 * (g1 - 128) + 0.436 * (b1 - 128) + 128))/2);
        *yuyv++ = clip(0.299 * (r1 - 128) + 0.587 * (g1 - 128) + 0.114 * (b1 - 128) + 128);
        *yuyv++ = clip(((0.615 * (r0 - 128) - 0.515 * (g0 - 128) - 0.100 * (b0 - 128) + 128) +
                       (0.615 * (r1 - 128) - 0.515 * (g1 - 128) - 0.100 * (b1 - 128) + 128))/2);
    }

    return numPixels * 2;
}


static
int rgb16_to_rgb24(u8* source, int sourceSize,
                   u8* dest, int destSize)
{
    throw eNotImplemented("This function will be lazy-implemented.");
    return 0;
}


static
int rgb16_to_rgba(u8* source, int sourceSize,
                   u8* dest, int destSize)
{
    throw eNotImplemented("This function will be lazy-implemented.");
    return 0;
}


static
int rgb16_to_yuyv(u8* source, int sourceSize,
                   u8* dest, int destSize)
{
    throw eNotImplemented("This function will be lazy-implemented.");
    return 0;
}


static
int rgb24_to_rgb16(u8* source, int sourceSize,
                   u8* dest, int destSize)
{
    throw eNotImplemented("This function will be lazy-implemented.");
    return 0;
}


static
int rgb24_to_rgba(u8* source, int sourceSize,
                   u8* dest, int destSize)
{
    throw eNotImplemented("This function will be lazy-implemented.");
    return 0;
}


static
int rgba_to_rgb16(u8* source, int sourceSize,
                   u8* dest, int destSize)
{
    throw eNotImplemented("This function will be lazy-implemented.");
    return 0;
}


static
int rgba_to_rgb24(u8* source, int sourceSize,
                   u8* dest, int destSize)
{
    throw eNotImplemented("This function will be lazy-implemented.");
    return 0;
}


static
int rgba_to_yuyv(u8* source, int sourceSize,
                   u8* dest, int destSize)
{
    throw eNotImplemented("This function will be lazy-implemented.");
    return 0;
}


static
int yuyv_to_rgb16(u8* source, int sourceSize,
                   u8* dest, int destSize)
{
    throw eNotImplemented("This function will be lazy-implemented.");
    return 0;
}


static
int yuyv_to_rgba(u8* source, int sourceSize,
                   u8* dest, int destSize)
{
    throw eNotImplemented("This function will be lazy-implemented.");
    return 0;
}

static
int rgb16_to_bgra(u8* source, int sourceSize,
                  u8* dest, int destSize)
{
    throw eNotImplemented("This function will be lazy-implemented.");
    return 0;
}

static
int rgb16_to_grey(u8* source, int sourceSize,
                  u8* dest, int destSize)
{
    throw eNotImplemented("This function will be lazy-implemented.");
    return 0;
}

static
int rgb24_to_bgra(u8* source, int sourceSize,
                  u8* dest, int destSize)
{
    throw eNotImplemented("This function will be lazy-implemented.");
    return 0;
}

static
int rgb24_to_grey(u8* source, int sourceSize,
                  u8* dest, int destSize)
{
    throw eNotImplemented("This function will be lazy-implemented.");
    return 0;
}

static
int rgba_to_bgra(u8* source, int sourceSize,
                  u8* dest, int destSize)
{
    throw eNotImplemented("This function will be lazy-implemented.");
    return 0;
}

static
int rgba_to_grey(u8* source, int sourceSize,
                  u8* dest, int destSize)
{
    throw eNotImplemented("This function will be lazy-implemented.");
    return 0;
}

static
int bgra_to_rgb16(u8* source, int sourceSize,
                  u8* dest, int destSize)
{
    throw eNotImplemented("This function will be lazy-implemented.");
    return 0;
}

static
int bgra_to_rgb24(u8* source, int sourceSize,
                  u8* dest, int destSize)
{
    if (sourceSize % 4)
        throw eLogicError("An BGRA buffer must be a multiple of 4.");
    int numPixels = sourceSize / 4;
    if (numPixels * 3 > destSize)
        throw eBufferOverflow("Not enough bytes in the RBG buffer.");
    for (int i = 0; i < destSize; i += 4)
    {
        u8 b = source[i+0];
        u8 g = source[i+1];
        u8 r = source[i+2];
        //u8 a = source[i+3];
        *dest++ = r;
        *dest++ = g;
        *dest++ = b;
    }
    return numPixels * 3;
}

static
int bgra_to_rgba(u8* source, int sourceSize,
                  u8* dest, int destSize)
{
    throw eNotImplemented("This function will be lazy-implemented.");
    return 0;
}

static
int bgra_to_yuyv(u8* source, int sourceSize,
                  u8* dest, int destSize)
{
    throw eNotImplemented("This function will be lazy-implemented.");
    return 0;
}

static
int bgra_to_grey(u8* source, int sourceSize,
                  u8* dest, int destSize)
{
    throw eNotImplemented("This function will be lazy-implemented.");
    return 0;
}

static
int yuyv_to_bgra(u8* source, int sourceSize,
                  u8* dest, int destSize)
{
    throw eNotImplemented("This function will be lazy-implemented.");
    return 0;
}

static
int yuyv_to_grey(u8* source, int sourceSize,
                  u8* dest, int destSize)
{
    throw eNotImplemented("This function will be lazy-implemented.");
    return 0;
}


typedef int (*converstion_func)(u8* source, int sourceSize, u8* dest, int destSize);


converstion_func** buildFuncMatrix()
{
    int numFormats = (int) kMaxImageFormat;

    converstion_func** matrix = new converstion_func*[numFormats];
    for (int i = 0; i < numFormats; i++)
        matrix[i] = new converstion_func[numFormats];

    for (int i = 0; i < numFormats; i++)
        matrix[i][i] = no_conversion;

    matrix[kRGB16][kRGB24] = rgb16_to_rgb24;
    matrix[kRGB16][kRGBA]  = rgb16_to_rgba;
    matrix[kRGB16][kBGRA]  = rgb16_to_bgra;
    matrix[kRGB16][kYUYV]  = rgb16_to_yuyv;
    matrix[kRGB16][kGrey]  = rgb16_to_grey;

    matrix[kRGB24][kRGB16] = rgb24_to_rgb16;
    matrix[kRGB24][kRGBA]  = rgb24_to_rgba;
    matrix[kRGB24][kBGRA]  = rgb24_to_bgra;
    matrix[kRGB24][kYUYV]  = rgb24_to_yuyv;
    matrix[kRGB24][kGrey]  = rgb24_to_grey;

    matrix[kRGBA][kRGB16]  = rgba_to_rgb16;
    matrix[kRGBA][kRGB24]  = rgba_to_rgb24;
    matrix[kRGBA][kBGRA]   = rgba_to_bgra;
    matrix[kRGBA][kYUYV]   = rgba_to_yuyv;
    matrix[kRGBA][kGrey]   = rgba_to_grey;

    matrix[kBGRA][kRGB16]  = bgra_to_rgb16;
    matrix[kBGRA][kRGB24]  = bgra_to_rgb24;
    matrix[kBGRA][kRGBA]   = bgra_to_rgba;
    matrix[kBGRA][kYUYV]   = bgra_to_yuyv;
    matrix[kBGRA][kGrey]   = bgra_to_grey;

    matrix[kYUYV][kRGB16]  = yuyv_to_rgb16;
    matrix[kYUYV][kRGB24]  = yuyv_to_rgb24;
    matrix[kYUYV][kRGBA]   = yuyv_to_rgba;
    matrix[kYUYV][kBGRA]   = yuyv_to_bgra;
    matrix[kYUYV][kGrey]   = yuyv_to_grey;

    return matrix;
}


void freeFuncMatrix(converstion_func** matrix)
{
    int numFormats = (int) kMaxImageFormat;
    for (int i = 0; i < numFormats; i++)
        delete [] matrix[i];
    delete [] matrix;
}


int colorspace_conversion(nImageFormat from, nImageFormat to,
                          u8* source, int sourceSize,
                          u8* dest, int destSize)
{
    int numFormats = (int) kMaxImageFormat;
    if (from >= numFormats || to >= numFormats || from < 0 || to < 0)
        throw eInvalidArgument("'from' or 'to' format is invalid");

    converstion_func** conversionFuncMatrix = buildFuncMatrix();
    int res = conversionFuncMatrix[from][to](source, sourceSize, dest, destSize);
    freeFuncMatrix(conversionFuncMatrix);

    return res;
}


}   // namespace img
}   // namespace rho
