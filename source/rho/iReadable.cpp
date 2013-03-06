#include <rho/iReadable.h>

#include <rho/eRho.h>

#include <errno.h>
#include <string.h>
#include <sstream>


namespace rho
{


tBufferedReadable::tBufferedReadable(
        iReadable* internalStream, u32 bufSize)
    : m_stream(internalStream), m_buf(NULL),
      m_bufSize(0), m_bufUsed(0), m_pos(0)
{
    if (m_stream == NULL)
        throw eNullPointer("internalStream must not be NULL.");
    m_buf = new u8[bufSize];
    m_bufSize = bufSize;
}

tBufferedReadable::~tBufferedReadable()
{
    delete [] m_buf;
    m_buf = NULL;
}

i32 tBufferedReadable::read(u8* buffer, i32 length)
{
    if (m_pos >= m_bufUsed)
        if (! refill())      // sets m_pos and m_bufUsed
            return -1;
    i32 i;
    for (i = 0; i < length && m_pos < m_bufUsed; i++)
        buffer[i] = m_buf[m_pos++];
    return i;
}

i32 tBufferedReadable::readAll(u8* buffer, i32 length)
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

bool tBufferedReadable::refill()
{
    m_pos = 0;
    m_bufUsed = 0;
    i32 r = m_stream->read(m_buf, m_bufSize);
    if (r < 0)
        return false;
    m_bufUsed = r;
    return true;
}


tFileReadable::tFileReadable(std::string filename)
    : m_filename(filename), m_file(NULL), m_eof(false)
{
    m_file = fopen(filename.c_str(), "rb");
    if (m_file == NULL)
    {
        std::ostringstream out;
        out << "Cannot open [" << filename << "] for reading (error: " << strerror(errno);
        throw eRuntimeError(out.str());
    }
}

tFileReadable::~tFileReadable()
{
    fclose(m_file);
    m_file = NULL;
}

i32 tFileReadable::read(u8* buffer, i32 length)
{
    size_t r = fread(buffer, 1, length, m_file);
    if (r > 0)
        return (i32)r;

    // r must be 0 at this point and onward.

    // If we've already seen the eof, return -1.
    if (m_eof)
        return -1;

    // Else, we haven't seen the eof before this point.
    if (feof(m_file))
    {
        m_eof = true;
        return 0;
    }
    else
    {
        std::ostringstream out;
        out << "Error reading [" << m_filename << "] (error: " << strerror(errno);
        throw eRuntimeError(out.str());
    }
}

i32 tFileReadable::readAll(u8* buffer, i32 length)
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

std::string tFileReadable::getFilename() const
{
    return m_filename;
}


}   // namespace rho
