#ifndef __rho_crypt_tWritableAES_h__
#define __rho_crypt_tWritableAES_h__


#include <rho/ppcheck.h>
#include <rho/iWritable.h>
#include <rho/iFlushable.h>
#include <rho/bNonCopyable.h>
#include <rho/crypt/tEncAES.h>


namespace rho
{
namespace crypt
{


/**
 * This writes data to an AES stream. It can be read by the tReadableAES class.
 *
 * See tReadableAES for details.
 */
class tWritableAES : public iWritable, public iFlushable, public bNonCopyable
{
    public:

        tWritableAES(iWritable* internalStream, nOperationModeAES opmode,
                     const u8 key[], nKeyLengthAES keylen);

        ~tWritableAES();

        i32 write(const u8* buffer, i32 length);
        i32 writeAll(const u8* buffer, i32 length);

        bool flush();

        void reset();

    private:

        // Stuff:
        iWritable* m_stream;
        u8* m_buf;
        u32 m_bufSize;   // <-- must be a multiple of the cypher block size
        u32 m_bufUsed;
        u64 m_seq;
        u8 m_parity[4];

        // Operation mode stuff:
        nOperationModeAES m_opmode;
        u8 m_last_ct[AES_BLOCK_SIZE];        // <-- only used in kOpModeCBC
        bool m_hasSentInitializationVector;  // <-- only used in kOpModeCBC

        // Encryption state:
        tEncAES m_aes;
};


}   // namespace crypt
}   // namespace rho


#endif   // __rho_crypt_tWritableAES_h__
