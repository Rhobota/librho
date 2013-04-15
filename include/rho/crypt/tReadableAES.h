#ifndef __rho_crypt_tReadableAES_h__
#define __rho_crypt_tReadableAES_h__


#include <rho/iReadable.h>
#include <rho/crypt/eKeyLength.h>

#ifndef AES_BLOCK_SIZE
#define AES_BLOCK_SIZE 16
#endif


namespace rho
{
namespace crypt
{


class tReadableAES : public iReadable
{
    public:

        tReadableAES(iReadable* internalStream,
                     const u8 key[], eKeyLengthAES keylen);

        ~tReadableAES();

        i32 read(u8* buffer, i32 length);
        i32 readAll(u8* buffer, i32 length);

    private:

        bool refill();

    private:

        // Stuff:
        tBufferedReadable m_stream;
        u8* m_buf;
        u32 m_bufSize;
        u32 m_bufUsed;
        u32 m_pos;
        u32 m_chunkBytesLeftToRead;

        // Encryption state:
        u32 m_rk[60];    // size is 4*(MAXNR+1)
        int m_Nr;
};


}    // namespace crypt
}    // namespace rho


#endif   // __rho_crypt_tReadableAES_h__
