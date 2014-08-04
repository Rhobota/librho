#include <rho/iReadable.h>

#include <rho/eRho.h>

#include <errno.h>
#include <string.h>
#include <sstream>
#include <fcntl.h>
#include <unistd.h>


namespace rho
{


tBufferedReadable::tBufferedReadable(
        iReadable* internalStream, u32 bufSize)
    : m_stream(internalStream), m_buf(NULL),
      m_bufSize(0), m_bufUsed(0), m_pos(0)
{
    if (m_stream == NULL)
        throw eNullPointer("internalStream must not be NULL.");
    if (bufSize == 0)
        throw eInvalidArgument("The buffer size must be positive.");
    m_buf = new u8[bufSize];
    m_bufSize = bufSize;
}

tBufferedReadable::~tBufferedReadable()
{
    m_stream = NULL;
    delete [] m_buf;
    m_buf = NULL;
    m_bufSize = 0;
    m_bufUsed = 0;
    m_pos = 0;
}

i32 tBufferedReadable::read(u8* buffer, i32 length)
{
    if (length <= 0)
        throw eInvalidArgument("Stream read/write length must be >0");

    if (m_pos >= m_bufUsed)
        if (! m_refill())      // sets m_pos and m_bufUsed
            return -1;
    i32 i;
    for (i = 0; i < length && m_pos < m_bufUsed; i++)
        buffer[i] = m_buf[m_pos++];
    return i;
}

i32 tBufferedReadable::readAll(u8* buffer, i32 length)
{
    if (length <= 0)
        throw eInvalidArgument("Stream read/write length must be >0");

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

bool tBufferedReadable::m_refill()
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
    #if __linux__ || __APPLE__ || __CYGWIN__
    int fd = open(filename.c_str(), O_RDONLY|O_CLOEXEC);
    if (fd >= 0)
        m_file = fdopen(fd, "rb");
    #elif __MINGW32__
    int fd = _open(filename.c_str(), _O_RDONLY|_O_BINARY);
    if (fd >= 0 && !SetHandleInformation(_get_osfhandle(fd), HANDLE_FLAG_INHERIT, 0))
        throw eRuntimeError("Cannot set CLOEXEC on file descriptor.");
    if (fd >= 0)
        m_file = _fdopen(fd, "rbc");
    #else
    #error What platform are you on!?
    #endif
    if (m_file == NULL)
    {
        if (fd >= 0)
            close(fd);
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
    if (length <= 0)
        throw eInvalidArgument("Stream read/write length must be >0");

    // If we've already seen the eof, return -1.
    if (m_eof)
        return -1;

    // Not found eof yet, so keep reading.
    size_t r = fread(buffer, 1, length, m_file);
    if (r > 0)
        return (i32)r;

    // r must be 0 at this point and onward.

    // We haven't seen the eof before this point, so check for that.
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
    if (length <= 0)
        throw eInvalidArgument("Stream read/write length must be >0");

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
