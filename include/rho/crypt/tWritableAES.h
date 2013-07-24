#ifndef __rho_crypt_tWritableAES_h__
#define __rho_crypt_tWritableAES_h__


#include <rho/iWritable.h>
#include <rho/iFlushable.h>
#include <rho/bNonCopyable.h>
#include <rho/crypt/nKeyLength.h>
#include <rho/crypt/nOperationModeAES.h>

#ifndef AES_BLOCK_SIZE
#define AES_BLOCK_SIZE 16
#endif


namespace rho
{
namespace crypt
{


class tWritableAES : public iWritable, public iFlushable, public bNonCopyable
{
    public:

        tWritableAES(iWritable* internalStream, nOperationModeAES opmode,
                     const u8 key[], nKeyLengthAES keylen);

        ~tWritableAES();

        i32 write(const u8* buffer, i32 length);
        i32 writeAll(const u8* buffer, i32 length);

        void flush();

    private:

        // Stuff:
        iWritable* m_stream;
        u8* m_buf;
        u32 m_bufSize;   // <-- must be a multiple of the cypher block size
        u32 m_bufUsed;

        // Operation mode stuff:
        nOperationModeAES m_opmode;
        u8 m_last_ct[AES_BLOCK_SIZE];        // <-- only used in kOpModeCBC
        bool m_hasSentInitializationVector;  // <-- only used in kOpModeCBC

        // Encryption state:
        u32 m_rk[60];    // size is 4*(MAXNR+1)
        int m_Nr;
};


}   // namespace crypt
}   // namespace rho


#endif   // __rho_crypt_tWritableAES_h__
