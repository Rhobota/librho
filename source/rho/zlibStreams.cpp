#include <rho/iReadable.h>
#include <rho/iWritable.h>

#include <zlib.h>
#include <string.h>


namespace rho
{


#define READ_CHUNK_SIZE 16384
#define WRITE_CHUNK_SIZE 16384


tZlibReadable::tZlibReadable(iReadable* internalStream)
    : m_stream(internalStream),
      m_zlibContext(NULL),
      m_inBuf(NULL),
      m_outBuf(NULL)
{
    z_stream ctx;
    memset(&ctx, 0, sizeof(ctx));
    ctx.zalloc = Z_NULL;
    ctx.zfree = Z_NULL;
    ctx.opaque = Z_NULL;
    if (inflateInit(&ctx) != Z_OK)
        throw eRuntimeError("Cannot create zlib context.");
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
}

i32 tZlibReadable::read(u8* buffer, i32 length)
{
    if (length <= 0)
        throw eInvalidArgument("Stream read/write length must be >0");
    if (buffer == NULL)
        throw eNullPointer("The read buffer may not be null.");

    //z_stream* ctx = (z_stream*) m_zlibContext;

    return 0;
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


tZlibWritable::tZlibWritable(iWritable* internalStream, int compressionLevel)
    : m_stream(internalStream),
      m_zlibContext(NULL),
      m_inBuf(NULL),
      m_outBuf(NULL)
{
    z_stream ctx;
    memset(&ctx, 0, sizeof(ctx));
    ctx.zalloc = Z_NULL;
    ctx.zfree = Z_NULL;
    ctx.opaque = Z_NULL;
    if (deflateInit(&ctx, compressionLevel) != Z_OK)
        throw eRuntimeError("Cannot create zlib context.");
    m_zlibContext = malloc(sizeof(ctx));
    memcpy(m_zlibContext, &ctx, sizeof(ctx));
    m_inBuf = new u8[WRITE_CHUNK_SIZE];
    m_outBuf = new u8[WRITE_CHUNK_SIZE];
}

tZlibWritable::~tZlibWritable()
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
            break;
        i32 have = WRITE_CHUNK_SIZE - (ctx->avail_out);
        if (have > 0)
            if (m_stream->writeAll(m_outBuf, have) != have)
                break;
    } while (ret != Z_STREAM_END);

    iFlushable* flushable = dynamic_cast<iFlushable*>(m_stream);
    if (flushable)
        flushable->flush();

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

    if (length > WRITE_CHUNK_SIZE)
        length = WRITE_CHUNK_SIZE;

    memcpy(m_inBuf, buffer, length);

    z_stream* ctx = (z_stream*) m_zlibContext;
    ctx->avail_in = length;
    ctx->next_in = m_inBuf;

    while ((ctx->avail_in) > 0)
    {
        ctx->avail_out = WRITE_CHUNK_SIZE;
        ctx->next_out = m_outBuf;
        int ret = deflate(ctx, Z_NO_FLUSH);
        if (ret != Z_OK)
            throw eRuntimeError("Cannot deflate data using zlib.");
        i32 have = WRITE_CHUNK_SIZE - (ctx->avail_out);
        if (have > 0)
            if (m_stream->writeAll(m_outBuf, have) != have)
                return 0;
    }

    return length;
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
    z_stream* ctx = (z_stream*) m_zlibContext;
    ctx->avail_in = 0;
    ctx->next_in = m_inBuf;
    do
    {
        ctx->avail_out = WRITE_CHUNK_SIZE;
        ctx->next_out = m_outBuf;
        int ret = deflate(ctx, Z_SYNC_FLUSH);
        if (ret != Z_OK)
            throw eRuntimeError("Cannot deflate data using zlib.");
        i32 have = WRITE_CHUNK_SIZE - (ctx->avail_out);
        if (have > 0)
            if (m_stream->writeAll(m_outBuf, have) != have)
                return false;
    } while ((ctx->avail_out) == 0);

    iFlushable* flushable = dynamic_cast<iFlushable*>(m_stream);
    if (flushable)
        return flushable->flush();
    else
        return true;
}


}   // namespace rho
