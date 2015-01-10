#include <rho/img/tVpxImageEncoder.h>
#include <rho/img/tVpxImageAsyncReadable.h>

#include <vpx/vpx_encoder.h>  // the base header for encoding with libvpx
#include <vpx/vp8cx.h>        // the specific encoder stuff for the vp8 and vp9 codecs

#include <sstream>


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
      m_tempImage(),
      m_codec(NULL)
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
    vpx_codec_ctx_t codec;
    res = vpx_codec_enc_init(&codec, codec_interface, &codec_config, 0);
    if (res != VPX_CODEC_OK)
    {
        std::ostringstream out;
        out << "Failed to init the codec [" << codec_name << "]. "
            << "Error: " << vpx_codec_err_to_string(res);
        throw eRuntimeError(out.str());
    }
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
