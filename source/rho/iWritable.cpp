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
    flush();
    m_stream = NULL;
    delete [] m_buf;
    m_buf = NULL;
    m_bufSize = 0;
    m_bufUsed = 0;
}

i32 tBufferedWritable::write(const u8* buffer, i32 length)
{
    if (m_bufUsed >= m_bufSize)
        flush();                     // <-- resets m_bufUsed to 0
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
    if (m_bufUsed == 0)
        return;
    i32 r = m_stream->writeAll(m_buf, m_bufUsed);
    if (r < 0 || ((u32)r) != m_bufUsed)
    {
        // This exception could be thrown through ~tBufferedWritable...
        // ...so that could be bad if an exception is already in flight... :(
        //throw eRuntimeError("Could not flush the buffered output stream!");
        return;
    }
    m_bufUsed = 0;
    iFlushable* flushable = dynamic_cast<iFlushable*>(m_stream);
    if (flushable)
        flushable->flush();
}


tFileWritable::tFileWritable(std::string filename)
    : m_filename(filename), m_file(NULL)
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
    fflush(m_file);
    fclose(m_file);
    m_file = NULL;
}

i32 tFileWritable::write(const u8* buffer, i32 length)
{
    return (i32) fwrite(buffer, 1, length, m_file);
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
    if (fflush(m_file) != 0)
    {
        std::ostringstream out;
        out << "Cannot flush writable file [" << m_filename << "] (error: " << strerror(errno);
        throw eRuntimeError(out.str());
    }
}

std::string tFileWritable::getFilename() const
{
    return m_filename;
}


}   // namespace rho
