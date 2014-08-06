#include <rho/iReadable.h>
#include <rho/iWritable.h>


namespace rho
{


tZlibReadable::tZlibReadable(iReadable* internalStream)
    : m_stream(internalStream),
      m_zlibContext(NULL),
      m_inBuf(NULL),
      m_outBuf(NULL)
{
}

tZlibReadable::~tZlibReadable()
{
}

i32 tZlibReadable::read(u8* buffer, i32 length)
{
}

i32 tZlibReadable::readAll(u8* buffer, i32 length)
{
}


tZlibWritable::tZlibWritable(iWritable* internalStream, int compressionLevel)
    : m_stream(internalStream),
      m_zlibContext(NULL),
      m_inBuf(NULL),
      m_outBuf(NULL)
{
}

tZlibWritable::~tZlibWritable()
{
}

i32 tZlibWritable::write(const u8* buffer, i32 length)
{
}

i32 tZlibWritable::writeAll(const u8* buffer, i32 length)
{
}

bool tZlibWritable::flush()
{
}


}   // namespace rho
