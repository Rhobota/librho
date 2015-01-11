#include <rho/img/tVpxImageEncoder.h>
#include <rho/img/tVpxImageAsyncReadable.h>

#include <vpx/vpx_encoder.h>  // the base header for encoding with libvpx
#include <vpx/vp8cx.h>        // the specific encoder stuff for the vp8 and vp9 codecs

#include <vpx/vpx_decoder.h>  // the base header for decoding with libvpx
#include <vpx/vp8dx.h>        // the specific decoder stuff for the vp8 and vp9 codecs

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


static
void s_flush(iWritable* writable)
{
    iFlushable* flushable = dynamic_cast<iFlushable*>(writable);
    if (flushable)
        if (!flushable->flush())
            throw eRuntimeError("Cannot flush the stream!");
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
      m_frameCount(0),
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
    try {
        s_flush(m_writable);
    } catch (...) { }

    if (m_vimage)
    {
        vpx_image_t* vimage = (vpx_image_t*)(m_vimage);
        vpx_img_free(vimage);
        delete vimage;
        vimage = NULL;
        m_vimage = NULL;
    }

    if (m_codec)
    {
        vpx_codec_ctx_t* codec = (vpx_codec_ctx_t*)(m_codec);
        vpx_codec_destroy(codec);
        delete codec;
        codec = NULL;
        m_codec = NULL;
    }
}

void tVpxImageEncoder::encodeImage(const tImage& image, bool flushWrites, bool forceKeyframe)
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

    // Encode this frame. (finally!)
    vpx_enc_frame_flags_t flags = 0;
    if (forceKeyframe)
        flags |= VPX_EFLAG_FORCE_KF;
    vpx_codec_err_t res = vpx_codec_encode(codec, vimage, m_frameCount++, 1, flags, VPX_DL_REALTIME);
    if (res != VPX_CODEC_OK)
    {
        std::ostringstream out;
        out << "Call to vpx_codec_encode() failed. "
            << "Error: " << vpx_codec_err_to_string(res);
        throw eRuntimeError(out.str());
    }

    // Grab the compressed bytes and write them out.
    vpx_codec_iter_t iter = NULL;
    const vpx_codec_cx_pkt_t* pkt = NULL;
    bool didWrite = false;
    while ((pkt = vpx_codec_get_cx_data(codec, &iter)) != NULL)
    {
        if (pkt->kind == VPX_CODEC_CX_FRAME_PKT)
        {
            //const int keyframe = (pkt->data.frame.flags & VPX_FRAME_IS_KEY) != 0;
            const u8* compressedBuf = (const u8*)(pkt->data.frame.buf);
            size_t compressedBufSize = pkt->data.frame.sz;
            //i64 thisFrameNumber = pkt->data.frame.pts;

            if (compressedBufSize <= 0)
                continue;

            didWrite = true;
            rho::pack(m_writable, (i32)compressedBufSize);
            i32 w = m_writable->writeAll(compressedBuf, (i32)compressedBufSize);
            if ((w < 0) || (((size_t)w) != compressedBufSize))
                throw eRuntimeError("Cannot write compressed data to underlying stream.");
        }
    }

    // Flush the stream.
    if (didWrite && flushWrites)
    {
        s_flush(m_writable);
    }
}

void tVpxImageEncoder::signalEndOfStream()
{
    vpx_codec_ctx_t* codec = (vpx_codec_ctx_t*)(m_codec);

    bool didWrite = true;

    while (didWrite)
    {
        didWrite = false;

        vpx_codec_err_t res = vpx_codec_encode(codec, NULL, -1, 1, 0, VPX_DL_REALTIME);
        if (res != VPX_CODEC_OK)
        {
            std::ostringstream out;
            out << "Call to vpx_codec_encode() failed. "
                << "Error: " << vpx_codec_err_to_string(res);
            throw eRuntimeError(out.str());
        }

        vpx_codec_iter_t iter = NULL;
        const vpx_codec_cx_pkt_t* pkt = NULL;
        while ((pkt = vpx_codec_get_cx_data(codec, &iter)) != NULL)
        {
            if (pkt->kind == VPX_CODEC_CX_FRAME_PKT)
            {
                //const int keyframe = (pkt->data.frame.flags & VPX_FRAME_IS_KEY) != 0;
                const u8* compressedBuf = (const u8*)(pkt->data.frame.buf);
                size_t compressedBufSize = pkt->data.frame.sz;
                //i64 thisFrameNumber = pkt->data.frame.pts;

                if (compressedBufSize <= 0)
                    continue;

                didWrite = true;
                rho::pack(m_writable, (i32)compressedBufSize);
                i32 w = m_writable->writeAll(compressedBuf, (i32)compressedBufSize);
                if ((w < 0) || (((size_t)w) != compressedBufSize))
                    throw eRuntimeError("Cannot write compressed data to underlying stream.");
            }
        }
    }

    s_flush(m_writable);
}


tVpxImageAsyncReadable::tVpxImageAsyncReadable(iAsyncReadableImageObserver* observer)
    : m_observer(observer),
      m_codec(NULL),
      m_image(),
      m_stage(kStageFrameSize),
      m_bufPos(0)
{
    // We use the vp9 decoder interface here because we use the vp9 encoder
    // in tVpxImageEncoder.
    vpx_codec_iface_t* codec_interface = vpx_codec_vp9_dx();

    // Get the name of our codec.
    std::string codec_name = vpx_codec_iface_name(codec_interface);

    // Init our decoder session.
    vpx_codec_ctx_t* codec = new vpx_codec_ctx_t;
    vpx_codec_err_t res = vpx_codec_dec_init(codec, codec_interface, NULL, 0);
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
