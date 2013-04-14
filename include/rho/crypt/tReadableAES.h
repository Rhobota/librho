#ifndef __rho_crypt_tReadableAES_h__
#define __rho_crypt_tReadableAES_h__


#include <rho/iReadable.h>
#include <rho/crypt/eKeyLength.h>


namespace rho
{
namespace crypt
{


class tReadableAES : public iReadable
{
    public:

        tReadableAES(iReadable* internalStream,
                     const u8 key[], eKeyLengthAES keylen);

        i32 read(u8* buffer, i32 length);
        i32 readAll(u8* buffer, i32 length);

    private:

        iReadable* m_stream;
};


}    // namespace crypt
}    // namespace rho


#endif   // __rho_crypt_tReadableAES_h__
