#ifndef __rho_crypt_tWritableAES_h__
#define __rho_crypt_tWritableAES_h__


#include <rho/iFlushable.h>
#include <rho/iWritable.h>
#include <rho/crypt/eKeyLength.h>


namespace rho
{
namespace crypt
{


class tWritableAES : public iWritable, public iFlushable
{
    public:

        tWritableAES(iWritable* internalStream,
                     const u8 key[], eKeyLengthAES keylen);

        ~tWritableAES();

        i32 write(const u8* buffer, i32 length);
        i32 writeAll(const u8* buffer, i32 length);

        void flush();

    private:

        iWritable* m_stream;
        u8* m_buf;
        u32 m_bufSize;   // <-- must be a multiple of the cypher block size
        u32 m_bufUsed;
};


}   // namespace crypt
}   // namespace rho


#endif   // __rho_crypt_tWritableAES_h__
