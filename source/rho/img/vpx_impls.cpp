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
void s_flush(iWritable* writable)
{
    iFlushable* flushable = dynamic_cast<iFlushable*>(writable);
    if (flushable)
        if (!flushable->flush())
            throw eRuntimeError("Cannot flush the stream!");
}


static
u32 s_toU32(u8 buf[4])
{
    u32 val = 0;
    val |= (buf[0] << 24);
    val |= (buf[1] << 16);
    val |= (buf[2] <<  8);
    val |= (buf[3] <<  0);
    return val;
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
        std::ostringstream out;
        out << "Failed to init the codec [" << codec_name << "]. "
            << "Error: " << vpx_codec_err_to_string(res) << ". "
            << "Error: " << vpx_codec_error_detail(codec);
        delete codec; codec = NULL;
        throw eRuntimeError(out.str());
    }
    m_codec = codec;

    // Alloc the image buffer space.
    vpx_image_t* vimage = new vpx_image_t;
    vpx_image_t* vimageDup = vpx_img_alloc(vimage,
                                           VPX_IMG_FMT_YV12,
                                           m_width,
                                           m_height,
                                           1);
    if (vimageDup != vimage)
    {
        delete vimage; vimage = NULL;
        delete codec; codec = NULL;
        m_codec = NULL;
        throw eRuntimeError("Cannot vpx_img_alloc()!");
    }
    m_vimage = vimage;
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
    // Copy the given image into the vimage buffer
    // (a conversion will be necessary for this).
    m_convertImage(image);

    // Grab references to our stuff.
    vpx_codec_ctx_t* codec = (vpx_codec_ctx_t*)(m_codec);
    vpx_image_t* vimage = (vpx_image_t*)(m_vimage);

    // Encode this frame.
    vpx_enc_frame_flags_t flags = 0;
    if (forceKeyframe)
        flags |= VPX_EFLAG_FORCE_KF;
    vpx_codec_err_t res = vpx_codec_encode(codec, vimage, m_frameCount++, 1, flags, VPX_DL_REALTIME);
    if (res != VPX_CODEC_OK)
    {
        std::ostringstream out;
        out << "Call to vpx_codec_encode() failed. "
            << "Error: " << vpx_codec_err_to_string(res) << ". "
            << "Error: " << vpx_codec_error_detail(codec);
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
                << "Error: " << vpx_codec_err_to_string(res) << ". "
                << "Error: " << vpx_codec_error_detail(codec);
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

void tVpxImageEncoder::m_convertImage(const tImage& image)
{
    // Check the image size.
    if (image.width() != m_width)
        throw eRuntimeError("The given image has the wrong width.");
    if (image.height() != m_height)
        throw eRuntimeError("The given image has the wrong height.");

    // We will expect all images to be in YUYV format.
    // (We might allow more input formats in the future...)
    if (image.format() != kYUYV)
        throw eRuntimeError("All images passed to encodeImage() must be in YUYV format.");

    // Converting to YV12 requires that the image have even dimensions.
    if ((m_width % 2) || (m_height % 2))
        throw eRuntimeError("The image must have even dimensions!");

    // Grab the reference to our vimage.
    vpx_image_t* vimage = (vpx_image_t*)(m_vimage);

    // Convert the image. The vimage is in YV12 format, so that's what we're headed for.
    const u8* srcBuf = image.buf();
    u8* yPlane = vimage->planes[VPX_PLANE_Y];
    u8* uPlane = vimage->planes[VPX_PLANE_U];
    u8* vPlane = vimage->planes[VPX_PLANE_V];
    int yPlaneStride = vimage->stride[VPX_PLANE_Y];
    int uPlaneStride = vimage->stride[VPX_PLANE_U];
    int vPlaneStride = vimage->stride[VPX_PLANE_V];
    u32 width = m_width;
    u32 height = m_height;
    for (u32 h = 0; h < height; h += 2)
    {
        for (u32 w = 0; w < width; w += 2)
        {
            u8 y0 = *srcBuf++;
            u8 u  = *srcBuf++;
            u8 y1 = *srcBuf++;
            u8 v  = *srcBuf++;
            yPlane[w] = y0;
            yPlane[w+1] = y1;
            uPlane[w>>1] = (u8)(u>>1);
            vPlane[w>>1] = (u8)(v>>1);
        }
        yPlane += yPlaneStride;
        for (u32 w = 0; w < width; w += 2)
        {
            u8 y0 = *srcBuf++;
            u8 u  = *srcBuf++;
            u8 y1 = *srcBuf++;
            u8 v  = *srcBuf++;
            yPlane[w] = y0;
            yPlane[w+1] = y1;
            uPlane[w>>1] = (u8)(uPlane[w>>1] + (u>>1));
            vPlane[w>>1] = (u8)(vPlane[w>>1] + (v>>1));
        }
        yPlane += yPlaneStride;
        uPlane += uPlaneStride;
        vPlane += vPlaneStride;
    }
}


tVpxImageAsyncReadable::tVpxImageAsyncReadable(iAsyncReadableImageObserver* observer)
    : m_observer(observer),
      m_codec(NULL),
      m_image(),
      m_stage(kStageFrameSize),
      m_bufPos(0),
      m_compressedBuf(NULL),
      m_compressedBufSize(0),
      m_compressedBufUsed(0)
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
        std::ostringstream out;
        out << "Failed to init the codec [" << codec_name << "]. "
            << "Error: " << vpx_codec_err_to_string(res) << ". "
            << "Error: " << vpx_codec_error_detail(codec);
        delete codec; codec = NULL;
        throw eRuntimeError(out.str());
    }
    m_codec = codec;
}

tVpxImageAsyncReadable::~tVpxImageAsyncReadable()
{
    if (m_codec)
    {
        vpx_codec_ctx_t* codec = (vpx_codec_ctx_t*)(m_codec);
        vpx_codec_destroy(codec);
        delete codec;
        codec = NULL;
        m_codec = NULL;
    }

    delete [] m_compressedBuf;
    m_compressedBuf = NULL;
    m_compressedBufSize = 0;
    m_compressedBufUsed = 0;
}

void tVpxImageAsyncReadable::takeInput(const u8* buffer, i32 length)
{
    if (length <= 0)
        throw eInvalidArgument("Stream read/write length must be >0");

    while (length > 0)
    {
        switch (m_stage)
        {
            case kStageFrameSize:
            {
                while (m_bufPos < 4 && length > 0)
                {
                    m_buf[m_bufPos++] = *buffer++;
                    --length;
                }
                if (m_bufPos == 4)
                {
                    m_bufPos = 0;
                    m_stage = kStageFrameData;
                    m_compressedBufUsed = s_toU32(m_buf);
                    if (m_compressedBufSize < m_compressedBufUsed)
                    {
                        delete [] m_compressedBuf;
                        m_compressedBufSize = 2*m_compressedBufUsed;
                        m_compressedBuf = new u8[m_compressedBufSize];
                    }
                }
                break;
            }

            case kStageFrameData:
            {
                u32 bufUsed = m_compressedBufUsed;
                u8* imagebuf = m_compressedBuf;
                while (m_bufPos < bufUsed && length > 0)
                {
                    imagebuf[m_bufPos++] = *buffer++;
                    --length;
                }
                if (m_bufPos == bufUsed)
                {
                    m_bufPos = 0;
                    m_stage = kStageFrameSize;
                    m_handleFrame();
                }
                break;
            }

            default:
            {
                throw eRuntimeError("Unknown state. How did this happen!?");
            }
        }
    }
}

void tVpxImageAsyncReadable::endStream()
{
    // TODO
}

static
void s_fillImage_fromYV12(vpx_image_t* vimage, tImage& image)
{
    // Convert vimage from YV12 to YUYV.
    u8* destBuf = image.buf();
    u8* yPlane = vimage->planes[VPX_PLANE_Y];
    u8* uPlane = vimage->planes[VPX_PLANE_U];
    u8* vPlane = vimage->planes[VPX_PLANE_V];
    int yPlaneStride = vimage->stride[VPX_PLANE_Y];
    int uPlaneStride = vimage->stride[VPX_PLANE_U];
    int vPlaneStride = vimage->stride[VPX_PLANE_V];
    u32 width = image.width();
    u32 height = image.height();
    for (u32 h = 0; h < height; h += 2)
    {
        for (u32 w = 0; w < width; w += 2)
        {
            *destBuf++ = yPlane[w];
            *destBuf++ = uPlane[w>>1];
            *destBuf++ = yPlane[w+1];
            *destBuf++ = vPlane[w>>1];
        }
        yPlane += yPlaneStride;
        for (u32 w = 0; w < width; w += 2)
        {
            *destBuf++ = yPlane[w];
            *destBuf++ = uPlane[w>>1];
            *destBuf++ = yPlane[w+1];
            *destBuf++ = vPlane[w>>1];
        }
        yPlane += yPlaneStride;
        uPlane += uPlaneStride;
        vPlane += vPlaneStride;
    }
}

static
void s_fillImage_fromI420(vpx_image_t* vimage, tImage& image)
{
    // TODO
}

static
void s_fillImage(vpx_image_t* vimage, tImage& image)
{
    // Ensure vimage is in the format we expect.
    switch (vimage->fmt)
    {
        case VPX_IMG_FMT_YV12:
            s_fillImage_fromYV12(vimage, image);
            break;

        case VPX_IMG_FMT_I420:
            s_fillImage_fromI420(vimage, image);
            break;

        default:
            throw eRuntimeError("Unexpected vimage format.");
    }
}

void tVpxImageAsyncReadable::m_handleFrame()
{
    // Decode the data we have.
    vpx_codec_ctx_t* codec = (vpx_codec_ctx_t*)(m_codec);
    vpx_codec_err_t res = vpx_codec_decode(codec, m_compressedBuf, m_compressedBufUsed, NULL, 0);
    if (res != VPX_CODEC_OK)
    {
        std::ostringstream out;
        out << "Call to vpx_codec_decode() failed. "
            << "Error: " << vpx_codec_err_to_string(res) << ". "
            << "Error: " << vpx_codec_error_detail(codec);
        throw eRuntimeError(out.str());
    }

    // Grab any frames that pop out of our data.
    vpx_codec_iter_t iter = NULL;
    vpx_image_t* vimage = NULL;
    while ((vimage = vpx_codec_get_frame(codec, &iter)) != NULL)
    {
        // Setup the image.
        m_image.setWidth(vimage->w);
        m_image.setHeight(vimage->h);
        m_image.setFormat(kYUYV);
        m_image.setBufUsed(m_image.width() * m_image.height() * 2);
        if (m_image.bufSize() < m_image.bufUsed())
            m_image.setBufSize(m_image.bufUsed());
        s_fillImage(vimage, m_image);

        // Notify the observer of this new frame.
        try {
            m_observer->gotImage(m_image);
        } catch (...) { }
    }
}


}  // namespace img
}  // namespace rho
