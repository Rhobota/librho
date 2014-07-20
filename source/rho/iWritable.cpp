#include <rho/iWritable.h>

#include <rho/eRho.h>

#include <errno.h>
#include <string.h>
#include <sstream>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>


namespace rho
{


tBufferedWritable::tBufferedWritable(
        iWritable* internalStream, u32 bufSize)
    : m_stream(internalStream), m_buf(NULL),
      m_bufSize(0), m_bufUsed(0)
{
    if (m_stream == NULL)
        throw eNullPointer("internalStream must not be NULL.");
    if (bufSize == 0)
        throw eInvalidArgument("The buffer size must be positive.");
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
    if (length <= 0)
        throw eInvalidArgument("Stream read/write length must be >0");

    if (m_bufUsed >= m_bufSize)
        if (! flush())    // <-- if successful, resets m_bufUsed to 0
            return 0;
    i32 i;
    for (i = 0; i < length && m_bufUsed < m_bufSize; i++)
        m_buf[m_bufUsed++] = buffer[i];
    return i;
}

i32 tBufferedWritable::writeAll(const u8* buffer, i32 length)
{
    if (length <= 0)
        throw eInvalidArgument("Stream read/write length must be >0");

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

bool tBufferedWritable::flush()
{
    if (m_bufUsed == 0)
        return true;

    i32 r = m_stream->writeAll(m_buf, m_bufUsed);
    if (r < 0 || ((u32)r) != m_bufUsed)
        return false;

    m_bufUsed = 0;

    iFlushable* flushable = dynamic_cast<iFlushable*>(m_stream);
    if (flushable)
        return flushable->flush();
    else
        return true;
}


tFileWritable::tFileWritable(std::string filename)
    : m_filename(filename), m_file(NULL), m_writeEOF(false)
{
    #if __linux__ || __APPLE__ || __CYGWIN__
    int fd = open(filename.c_str(), O_WRONLY|O_CREAT|O_TRUNC|O_CLOEXEC, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH);
    #elif __MINGW32__
    int fd = open(filename.c_str(), O_WRONLY|O_CREAT|O_TRUNC, S_IRUSR|S_IWUSR);
    if (fd >= 0 && !SetHandleInformation((HANDLE)fd, HANDLE_FLAG_INHERIT, 0))
        throw eRuntimeError("Cannot set CLOEXEC on file descriptor.");
    #else
    #error What platform are you on!?
    #endif
    if (fd >= 0)
        m_file = fdopen(fd, "wb");
    if (m_file == NULL)
    {
        if (fd >= 0)
            close(fd);
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
    if (length <= 0)
        throw eInvalidArgument("Stream read/write length must be >0");

    if (m_writeEOF)
        return 0;

    size_t w = fwrite(buffer, 1, length, m_file);
    if (w > 0)
        return (i32)w;

    m_writeEOF = true;
    return 0;
}

i32 tFileWritable::writeAll(const u8* buffer, i32 length)
{
    if (length <= 0)
        throw eInvalidArgument("Stream read/write length must be >0");

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

bool tFileWritable::flush()
{
    return (fflush(m_file) == 0);
}

std::string tFileWritable::getFilename() const
{
    return m_filename;
}


}   // namespace rho
