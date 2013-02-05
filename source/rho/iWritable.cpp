#include <rho/iWritable.h>

#include <rho/eRho.h>

#include <errno.h>
#include <string.h>
#include <sstream>


namespace rho
{


tBufferedWritable::tBufferedWritable(
        iWritable* internalStream, u32 bufSize)
    : m_stream(internalStream), m_buf(NULL),
      m_bufSize(0), m_bufUsed(0)
{
    if (m_stream == NULL)
        throw eNullPointer("internalStream must not be NULL.");
    m_buf = new u8[bufSize];
    m_bufSize = bufSize;
}

tBufferedWritable::~tBufferedWritable()
{
    delete [] m_buf;
}

i32 tBufferedWritable::write(const u8* buffer, i32 length)
{
    if (m_bufUsed >= m_bufSize)
        flush();
    i32 i;
    for (i = 0; i < length && m_bufUsed < m_bufSize; i++)
        m_buf[m_bufUsed++] = buffer[i];
    return i;
}

i32 tBufferedWritable::writeAll(const u8* buffer, i32 length)
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

void tBufferedWritable::flush()
{
    i32 r = m_stream->writeAll(m_buf, m_bufUsed);
    if (r < 0 || ((u32)r) != m_bufUsed)
        throw eRuntimeError("Could not flush the buffered output stream!");
    m_bufUsed = 0;
    iFlushable* flushable = dynamic_cast<iFlushable*>(m_stream);
    if (flushable)
        flushable->flush();
}


tFileWritable::tFileWritable(std::string filename)
    : m_file(NULL)
{
    m_file = fopen(filename.c_str(), "wb");
    if (m_file == NULL)
    {
        std::ostringstream out;
        out << "Cannot open [" << filename << "] for writing (error: " << strerror(errno);
        throw eRuntimeError(out.str());
    }
}

tFileWritable::~tFileWritable()
{
    fclose(m_file);
    m_file = NULL;
}

i32 tFileWritable::write(const u8* buffer, i32 length)
{
    return fwrite(buffer, 1, length, m_file);
}

i32 tFileWritable::writeAll(const u8* buffer, i32 length)
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

void tFileWritable::flush()
{
    fflush(m_file);
}


}   // namespace rho
