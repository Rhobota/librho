#include <rho/crypt/tReadableAES.h>


namespace rho
{
namespace crypt
{


tReadableAES::tReadableAES(iReadable* internalStream,
             const u8 key[], eKeyLengthAES keylen)
{
}

i32 tReadableAES::read(u8* buffer, i32 length)
{
    return length;
}

i32 tReadableAES::readAll(u8* buffer, i32 length)
{
    return length;
}


}    // namespace crypt
}    // namespace rho
