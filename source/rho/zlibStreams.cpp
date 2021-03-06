#include <rho/iAsyncReadable.h>
#include <rho/iReadable.h>
#include <rho/iWritable.h>

#include <string.h>

#include "zlib_source/zlib-1.2.8/zlib.h"


namespace rho
{


#define READ_CHUNK_SIZE 65000
#define WRITE_CHUNK_SIZE 65000


///////////////////////////////////////////////////////////////////////////////
// tZlibReadable
///////////////////////////////////////////////////////////////////////////////

tZlibReadable::tZlibReadable(iReadable* internalStream)
    : m_stream(internalStream),
      m_zlibContext(NULL),
      m_inBuf(NULL),
      m_outBuf(NULL),
      m_outUsed(0),
      m_outPos(0),
      m_eof(false)
{
    z_stream ctx;
    memset(&ctx, 0, sizeof(ctx));
    ctx.zalloc = Z_NULL;
    ctx.zfree = Z_NULL;
    ctx.opaque = Z_NULL;
    int ret = inflateInit(&ctx);
    if (ret != Z_OK)
        throw eRuntimeError(std::string("Zlib error: ") + zError(ret));
    ctx.avail_in = ctx.avail_out = 0;
    ctx.next_in = ctx.next_out = NULL;
    m_zlibContext = malloc(sizeof(ctx));
    memcpy(m_zlibContext, &ctx, sizeof(ctx));
    m_inBuf = new u8[READ_CHUNK_SIZE];
    m_outBuf = new u8[READ_CHUNK_SIZE];
}

tZlibReadable::~tZlibReadable()
{
    inflateEnd((z_stream*)m_zlibContext);
    free(m_zlibContext);
    m_zlibContext = NULL;
    delete [] m_inBuf;
    m_inBuf = NULL;
    delete [] m_outBuf;
    m_outBuf = NULL;
    m_stream = NULL;
    m_outUsed = 0;
    m_outPos = 0;
}

i32 tZlibReadable::read(u8* buffer, i32 length)
{
    if (length <= 0)
        throw eInvalidArgument("Stream read/write length must be >0");

    if (m_outPos >= m_outUsed)
        if (! m_refill())      // sets m_outPos and m_outUsed
            return -1;

    u32 rem = m_outUsed - m_outPos;
    if (rem > (u32)length)
        rem = (u32)length;

    memcpy(buffer, m_outBuf+m_outPos, rem);
    m_outPos += rem;

    return (i32)rem;
}

i32 tZlibReadable::readAll(u8* buffer, i32 length)
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

bool tZlibReadable::m_refill()
{
    if (m_eof)
        return false;

    m_outPos = 0;
    m_outUsed = 0;

    while (m_outUsed == 0 && !m_eof)
    {
        // If this is being called, it means we've
        // read everything that's available in the
        // out-buffer. In that case, we can reset it
        // and potentially re-use the entire thing.
        z_stream* ctx = (z_stream*) m_zlibContext;
        ctx->avail_out = READ_CHUNK_SIZE;
        ctx->next_out = m_outBuf;

        // If there's nothing in the in-buffer, we need
        // to try to obtain more from the underlying stream.
        if ((ctx->avail_in) == 0)
        {
            i32 r = m_stream->read(m_inBuf, READ_CHUNK_SIZE);
            if (r > 0)
            {
                ctx->avail_in = r;
            }
            ctx->next_in = m_inBuf;
        }

        // Now we can try to decompress stuff.
        //
        // inflate() will return:
        //     - Z_STREAM_END if all output has been produced, or
        //     - Z_OK if the stream is still valid and progress was made, or
        //     - an error if (1) the stream is invalid or (2) the stream
        //       seems valid but no progress was made.
        //
        int ret = inflate(ctx, Z_SYNC_FLUSH);
        if (ret == Z_STREAM_END)
            m_eof = true;
        else if (ret != Z_OK)
            throw eRuntimeError(std::string("Zlib error: ") + zError(ret));
        m_outUsed = READ_CHUNK_SIZE - (ctx->avail_out);
    }

    // m_outUsed must be positive, or we found the eof, so we're good to go.
    return true;
}


///////////////////////////////////////////////////////////////////////////////
// tZlibAsyncReadable
///////////////////////////////////////////////////////////////////////////////

tZlibAsyncReadable::tZlibAsyncReadable(iAsyncReadable* nextReadable)
    : m_nextReadable(nextReadable),
      m_zlibContext(NULL),
      m_outBuf(NULL),
      m_eof(false)
{
    if (!m_nextReadable)
        throw eInvalidArgument("nextReadable should not be NULL!");
    z_stream ctx;
    memset(&ctx, 0, sizeof(ctx));
    ctx.zalloc = Z_NULL;
    ctx.zfree = Z_NULL;
    ctx.opaque = Z_NULL;
    int ret = inflateInit(&ctx);
    if (ret != Z_OK)
        throw eRuntimeError(std::string("Zlib error: ") + zError(ret));
    ctx.avail_in = ctx.avail_out = 0;
    ctx.next_in = ctx.next_out = NULL;
    m_zlibContext = malloc(sizeof(ctx));
    memcpy(m_zlibContext, &ctx, sizeof(ctx));
    m_outBuf = new u8[READ_CHUNK_SIZE];
}

tZlibAsyncReadable::~tZlibAsyncReadable()
{
    inflateEnd((z_stream*)m_zlibContext);
    free(m_zlibContext);
    m_zlibContext = NULL;
    delete [] m_outBuf;
    m_outBuf = NULL;
    m_nextReadable = NULL;
}

void tZlibAsyncReadable::takeInput(const u8* buffer, i32 length)
{
    if (length <= 0)
        throw eInvalidArgument("Stream read/write length must be >0");

    if (m_eof)
        return;

    z_stream* ctx = (z_stream*) m_zlibContext;
    ctx->avail_in = length;
    ctx->next_in = buffer;
    ctx->avail_out = READ_CHUNK_SIZE;
    ctx->next_out = m_outBuf;

    while (ctx->avail_in > 0 || ctx->avail_out == 0)
    {
        ctx->avail_out = READ_CHUNK_SIZE;
        ctx->next_out = m_outBuf;

        int ret = inflate(ctx, Z_SYNC_FLUSH);
        if (ret == Z_STREAM_END)
            m_eof = true;
        else if (ret != Z_OK && ret != Z_BUF_ERROR)
            throw eRuntimeError(std::string("Zlib error: ") + zError(ret));

        i32 outUsed = READ_CHUNK_SIZE - (ctx->avail_out);

        if (outUsed > 0)
            m_nextReadable->takeInput(m_outBuf, outUsed);

        if (m_eof)
        {
            m_nextReadable->endStream();
            break;
        }
    }
}

void tZlibAsyncReadable::endStream()
{
    if (!m_eof)
    {
        // This is bad! With a zlib stream, there is an EOS marker that
        // we should have found inside takeInput(). But we didn't. So
        // we'll set eof here and throw.
        m_eof = true;
        m_nextReadable->endStream();
        throw eRuntimeError("Zlib error: no EOS marker found");
    }
}


///////////////////////////////////////////////////////////////////////////////
// tZlibWritable
///////////////////////////////////////////////////////////////////////////////

tZlibWritable::tZlibWritable(iWritable* internalStream, int compressionLevel)
    : m_stream(internalStream),
      m_zlibContext(NULL),
      m_inBuf(NULL),
      m_outBuf(NULL),
      m_broken(false),
      m_inBufPos(0)
{
    z_stream ctx;
    memset(&ctx, 0, sizeof(ctx));
    ctx.zalloc = Z_NULL;
    ctx.zfree = Z_NULL;
    ctx.opaque = Z_NULL;
    int ret = deflateInit(&ctx, compressionLevel);
    if (ret != Z_OK)
        throw eRuntimeError(std::string("Zlib error: ") + zError(ret));
    m_zlibContext = malloc(sizeof(ctx));
    memcpy(m_zlibContext, &ctx, sizeof(ctx));
    m_inBuf = new u8[WRITE_CHUNK_SIZE];
    m_outBuf = new u8[WRITE_CHUNK_SIZE];
}

tZlibWritable::~tZlibWritable()
{
    try
    {
        if (m_inBufPos > 0)
            this->flush();
        this->close();
    }
    catch (...)
    {
        // Well... we tried.
    }

    deflateEnd((z_stream*)m_zlibContext);
    free(m_zlibContext);
    m_zlibContext = NULL;
    delete [] m_inBuf;
    m_inBuf = NULL;
    delete [] m_outBuf;
    m_outBuf = NULL;
    m_stream = NULL;
}

i32 tZlibWritable::write(const u8* buffer, i32 length)
{
    if (length <= 0)
        throw eInvalidArgument("Stream read/write length must be >0");
    if (buffer == NULL)
        throw eNullPointer("The write buffer may not be null.");

    if (m_broken)
        return 0;

    if (m_inBufPos >= WRITE_CHUNK_SIZE)
        if (!flush())   // <-- resets m_inBufPos if successful
            return 0;

    i32 rem = WRITE_CHUNK_SIZE - m_inBufPos;
    if (rem > length)
        rem = length;

    memcpy(m_inBuf+m_inBufPos, buffer, rem);
    m_inBufPos += rem;

    return rem;
}

i32 tZlibWritable::writeAll(const u8* buffer, i32 length)
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

bool tZlibWritable::flush()
{
    if (m_broken)
        return false;

    z_stream* ctx = (z_stream*) m_zlibContext;
    ctx->avail_in = m_inBufPos;
    ctx->next_in = m_inBuf;
    m_inBufPos = 0;

    while ((ctx->avail_in) > 0)
    {
        ctx->avail_out = WRITE_CHUNK_SIZE;
        ctx->next_out = m_outBuf;
        int ret = deflate(ctx, Z_NO_FLUSH);
        if (ret != Z_OK)
            throw eRuntimeError(std::string("Zlib error: ") + zError(ret));
        i32 have = WRITE_CHUNK_SIZE - (ctx->avail_out);
        if (have > 0 && m_stream->writeAll(m_outBuf, have) != have)
        {
            m_broken = true;
            return false;
        }
    }

    ctx->avail_in = 0;
    ctx->next_in = m_inBuf;
    do
    {
        ctx->avail_out = WRITE_CHUNK_SIZE;
        ctx->next_out = m_outBuf;
        int ret = deflate(ctx, Z_SYNC_FLUSH);
        if (ret != Z_OK && ret != Z_BUF_ERROR)
            throw eRuntimeError(std::string("Zlib error: ") + zError(ret));
        i32 have = WRITE_CHUNK_SIZE - (ctx->avail_out);
        if (have > 0 && m_stream->writeAll(m_outBuf, have) != have)
        {
            m_broken = true;
            return false;
        }
    } while ((ctx->avail_out) == 0);

    iFlushable* flushable = dynamic_cast<iFlushable*>(m_stream);
    if (flushable)
    {
        if (!flushable->flush())
        {
            m_broken = true;
            return false;
        }
        else
            return true;
    }
    else
        return true;
}

void tZlibWritable::close()
{
    if (!m_broken)
    {
        z_stream* ctx = (z_stream*) m_zlibContext;
        ctx->avail_in = 0;
        ctx->next_in = m_inBuf;
        int ret;
        do
        {
            ctx->avail_out = WRITE_CHUNK_SIZE;
            ctx->next_out = m_outBuf;
            ret = deflate(ctx, Z_FINISH);
            if (ret != Z_OK && ret != Z_STREAM_END)
                break;   // zlib error occurred!
            i32 have = WRITE_CHUNK_SIZE - (ctx->avail_out);
            if (have > 0 && m_stream->writeAll(m_outBuf, have) != have)
            {
                m_broken = true;
                break;    // underlying stream error occurred!
            }
        } while (ret != Z_STREAM_END);
    }

    if (!m_broken)
    {
        iFlushable* flushable = dynamic_cast<iFlushable*>(m_stream);
        if (flushable)
            flushable->flush();
    }

    m_broken = true;

    iClosable* closable = dynamic_cast<iClosable*>(m_stream);
    if (closable)
        closable->close();
}


}   // namespace rho
