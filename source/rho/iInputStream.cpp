#include <rho/iInputStream.h>

#include <rho/eRho.h>


namespace rho
{


tBufferedInputStream::tBufferedInputStream(
        iInputStream* internalStream, u32 bufSize)
    : m_stream(internalStream), m_buf(NULL),
      m_bufSize(0), m_bufUsed(0), m_pos(0)
{
    if (m_stream == NULL)
        throw eNullPointer("internalStream must not be NULL.");
    m_buf = new u8[bufSize];
    m_bufSize = bufSize;
}

tBufferedInputStream::~tBufferedInputStream()
{
    delete [] m_buf;
}

i32 tBufferedInputStream::read(u8* buffer, i32 length)
{
    if (m_pos >= m_bufUsed)
        if (! refill())
            return -1;
    i32 i;
    for (i = 0; i < length && m_pos < m_bufUsed; i++)
        buffer[i] = m_buf[m_pos++];
    return i;
}

i32 tBufferedInputStream::readAll(u8* buffer, i32 length)
{
    i32 amountRead = 0;
    while (amountRead < length)
    {
        i32 n = read(buffer+amountRead, length-amountRead);
        if (n <= 0)
            return (amountRead>0) ? amountRead : n;
        amountRead += n;
    }
    return amountRead;
}

bool tBufferedInputStream::refill()
{
    m_pos = 0;
    m_bufUsed = 0;
    i32 r = m_stream->read(m_buf, m_bufSize);
    if (r < 0)
        return false;
    m_bufUsed = r;
    return true;
}


}   // namespace rho
