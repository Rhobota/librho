#include <rho/img/tVpxImageEncoder.h>
#include <rho/img/tVpxImageAsyncReadable.h>

#include <vpx/vpx_encoder.h>  // the base header for encoding with libvpx
#include <vpx/vp8cx.h>        // the specific encoder stuff for the vp8 and vp9 codecs

#include <sstream>


namespace rho
{
namespace img
{


static
vpx_img_fmt_t s_rhoFormatToVpxFormat(nImageFormat fmt)
{
    switch (fmt)
    {
        case kRGB16:
            return VPX_IMG_FMT_RGB565;
        case kRGB24:
            return VPX_IMG_FMT_RGB24;
        case kBGRA:
            return VPX_IMG_FMT_ARGB_LE;
        case kYUYV:
            return VPX_IMG_FMT_YUY2;

        case kGrey:
        case kRGBA:
        default:
        {
            std::ostringstream out;
            out << "Cannot convert nImageFormat to vpx_img_fmt_t. Input: " << ((u32)fmt);
            throw eRuntimeError(out.str());
        }
    }
}


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
      m_tempImage(),
      m_codec(NULL),
      m_vimage(NULL)
{
    // This will hold the returns values of the calls below (it's an enum).
    // Use vpx_codec_err_to_string() to convert its value to a string.
    vpx_codec_err_t res = VPX_CODEC_OK;

    // Declare which codec we want to use for encoding.
    // We will use vp9 because it is the current best
    // that libvpx offers.
    vpx_codec_iface_t* codec_interface = vpx_codec_vp9_cx();

    // Grab the name of the codec we're using.
    std::string codec_name = vpx_codec_iface_name(codec_interface);

    // Grab the default configuration for our codec.
    vpx_codec_enc_cfg_t codec_config;
    res = vpx_codec_enc_config_default(codec_interface, &codec_config, 0);
    if (res != VPX_CODEC_OK)
    {
        std::ostringstream out;
        out << "Failed to get default config for codec [" << codec_name << "]. "
            << "Error: " << vpx_codec_err_to_string(res);
        throw eRuntimeError(out.str());
    }

    // Customize the configuration for our needs.
    codec_config.g_w = m_width;
    codec_config.g_h = m_height;
    codec_config.g_timebase.num = 1;
    codec_config.g_timebase.den = m_fps;
    codec_config.rc_target_bitrate = m_bitrate;
    codec_config.g_error_resilient = 0;

    // Init the codec.
    vpx_codec_ctx_t* codec = new vpx_codec_ctx_t;
    res = vpx_codec_enc_init(codec, codec_interface, &codec_config, 0);
    if (res != VPX_CODEC_OK)
    {
        delete codec; codec = NULL;
        std::ostringstream out;
        out << "Failed to init the codec [" << codec_name << "]. "
            << "Error: " << vpx_codec_err_to_string(res);
        throw eRuntimeError(out.str());
    }
    m_codec = codec;
}

tVpxImageEncoder::~tVpxImageEncoder()
{
    if (m_codec)
    {
        vpx_codec_ctx_t* codec = (vpx_codec_ctx_t*)(m_codec);
        // TODO -- call whatever dealloc method we need to on "codec"
        delete codec;
        codec = NULL;
        m_codec = NULL;
    }

    if (m_vimage)
    {
        vpx_image_t* vimage = (vpx_image_t*)(m_vimage);
        vpx_img_free(vimage);
        delete vimage;
        vimage = NULL;
        m_vimage = NULL;
    }
}

void tVpxImageEncoder::encodeImage(const tImage& image)
{
    // Check the image size.
    if (image.width() != m_width)
        throw eRuntimeError("The given image has the wrong width.");
    if (image.height() != m_height)
        throw eRuntimeError("The given image has the wrong height.");

    // If this is the first call, we need to allocate m_vimage.
    if (!m_vimage)
    {
        vpx_image_t* vimage = new vpx_image_t;
        vpx_image_t* vimageDup = vpx_img_alloc(vimage,
                                               s_rhoFormatToVpxFormat(image.format()),
                                               image.width(),
                                               image.height(),
                                               1);
        if (vimageDup != vimage)
        {
            delete vimage; vimage = NULL;
            throw eRuntimeError("Cannot vpx_img_alloc()!");
        }
        m_vimage = vimage;
    }

    // Grab references to our stuff.
    vpx_codec_ctx_t* codec = (vpx_codec_ctx_t*)(m_codec);
    vpx_image_t* vimage = (vpx_image_t*)(m_vimage);

    // Ensure the image we are given here has the format we expect.
    if (s_rhoFormatToVpxFormat(image.format()) != vimage->fmt)
    {
        throw eRuntimeError("The given image has the wrong format. "
                "The correct format is set on this first call to this method. "
                "All subsequent calls must use the same format as the first one.");
    }

    // Copy the given image into the vimage buffer.
    u8* imagebuf = vimage->planes[VPX_PLANE_PACKED];
    int stride = vimage->stride[VPX_PLANE_PACKED];
    int imageBPP = (int)getBPP(image.format());
    if (vimage->w != m_width)
        throw eRuntimeError("The width of vimage is unexpected.");
    if (vimage->h != m_height)
        throw eRuntimeError("The height of vimage is unexpected.");
    if (imageBPP * 8 != vimage->bps)
        throw eRuntimeError("The bites-per-sample of vimage is unexpected.");
    if (stride != (int)m_width * imageBPP)
        throw eRuntimeError("The stride of vimage is unexpected.");
    if (m_width * m_height * imageBPP != image.bufUsed())
        throw eRuntimeError("The bufUsed() of image is unexpected.");
    memcpy(imagebuf, image.buf(), image.bufUsed());
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
