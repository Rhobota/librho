#include <rho/iOutputStream.h>

#include <rho/eRho.h>


namespace rho
{


tBufferedOutputStream::tBufferedOutputStream(
        iOutputStream* internalStream, u32 bufSize)
    : m_stream(internalStream), m_buf(NULL),
      m_bufSize(0), m_bufUsed(0)
{
    if (m_stream == NULL)
        throw eNullPointer("internalStream must not be NULL.");
    m_buf = new u8[bufSize];
    m_bufSize = bufSize;
}

tBufferedOutputStream::~tBufferedOutputStream()
{
    delete [] m_buf;
}

i32 tBufferedOutputStream::write(const u8* buffer, i32 length)
{
    if (m_bufUsed >= m_bufSize)
        flush();
    i32 i;
    for (i = 0; i < length && m_bufUsed < m_bufSize; i++)
        m_buf[m_bufUsed++] = buffer[i];
    return i;
}

i32 tBufferedOutputStream::writeAll(const u8* buffer, i32 length)
{
    i32 amountWritten = 0;
    while (amountWritten < length)
    {
        i32 n = write(buffer+amountWritten, length-amountWritten);
        if (n <= 0)
            return (amountWritten>0) ? amountWritten : n;
        amountWritten += n;
    }
    return amountWritten;
}

void tBufferedOutputStream::flush()
{
    i32 r = m_stream->writeAll(m_buf, m_bufUsed);
    if (r < 0 || ((u32)r) != m_bufUsed)
        throw eRuntimeError("Could not flush the buffered output stream!");
    m_bufUsed = 0;
}


}   // namespace rho
