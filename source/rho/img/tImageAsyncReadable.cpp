#include <rho/img/tImageAsyncReadable.h>


namespace rho
{
namespace img
{


tImageAsyncReadable::tImageAsyncReadable(iAsyncReadableImageObserver* observer)
    : m_observer(observer),
      m_image(),
      m_stage(kStageBufUsed),
      m_bufPos(0)
{
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

void tImageAsyncReadable::takeInput(const u8* buffer, i32 length)
{
    if (length <= 0)
        throw eInvalidArgument("Stream read/write length must be >0");

    while (length > 0)
    {
        switch (m_stage)
        {
            case kStageBufUsed:
            {
                while (m_bufPos < 4 && length > 0)
                {
                    m_buf[m_bufPos++] = *buffer++;
                    --length;
                }
                if (m_bufPos == 4)
                {
                    m_bufPos = 0;
                    m_stage = kStageBufData;
                    m_image.setBufUsed(s_toU32(m_buf));
                    if (m_image.bufSize() < m_image.bufUsed())
                        m_image.setBufSize(m_image.bufUsed());
                }
                break;
            }

            case kStageBufData:
            {
                u32 bufUsed = m_image.bufUsed();
                u8* imagebuf = m_image.buf();
                while (m_bufPos < bufUsed && length > 0)
                {
                    imagebuf[m_bufPos++] = *buffer++;
                    --length;
                }
                if (m_bufPos == bufUsed)
                {
                    m_bufPos = 0;
                    m_stage = kStageWidth;
                }
                break;
            }

            case kStageWidth:
            {
                while (m_bufPos < 4 && length > 0)
                {
                    m_buf[m_bufPos++] = *buffer++;
                    --length;
                }
                if (m_bufPos == 4)
                {
                    m_bufPos = 0;
                    m_stage = kStageHeight;
                    m_image.setWidth(s_toU32(m_buf));
                }
                break;
            }

            case kStageHeight:
            {
                while (m_bufPos < 4 && length > 0)
                {
                    m_buf[m_bufPos++] = *buffer++;
                    --length;
                }
                if (m_bufPos == 4)
                {
                    m_bufPos = 0;
                    m_stage = kStageFormat;
                    m_image.setHeight(s_toU32(m_buf));
                }
                break;
            }

            case kStageFormat:
            {
                while (m_bufPos < 4 && length > 0)
                {
                    m_buf[m_bufPos++] = *buffer++;
                    --length;
                }
                if (m_bufPos == 4)
                {
                    m_bufPos = 0;
                    m_stage = kStageBufUsed;
                    m_image.setFormat((nImageFormat)(s_toU32(m_buf)));
                    try {
                        m_observer->gotImage(&m_image);
                    } catch (...) { }
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

void tImageAsyncReadable::endStream()
{
    // Nothing to do here.
}


}  // namespace img
}  // namespace rho
