#include <rho/crypt/tWritableAES.h>


namespace rho
{
namespace crypt
{


tWritableAES::tWritableAES(iWritable* internalStream,
             const u8 key[], eKeyLengthAES keylen)
{
}

i32 tWritableAES::write(const u8* buffer, i32 length)
{
    return length;
}

i32 tWritableAES::writeAll(const u8* buffer, i32 length)
{
    return length;
}

void tWritableAES::flush()
{
}


}   // namespace crypt
}   // namespace rho
