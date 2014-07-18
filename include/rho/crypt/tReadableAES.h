#ifndef __rho_crypt_tReadableAES_h__
#define __rho_crypt_tReadableAES_h__


#include <rho/ppcheck.h>
#include <rho/iReadable.h>
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


/**
 * This class reads from an AES stream created by tWritableAES.
 *
 * This class provides many guarantees about the data that
 * is returned by the two read() methods. The guarantees
 * basically sum up to be "the data is legitimate, complete,
 * and unaltered". Details follow.
 *
 * Assumptions:
 *    - CBC mode is being used
 *    - the key is only know by the sender and the receiver
 *    - the key has never been used before  <--  IMPORTANT!
 *
 * Guarantees:
 *    - byte injection will fail the connection
 *        * the 4-byte parity check will ensure this
 *        * all bytes after the injection will be
 *          decrypted incorrectly because of CBC mode,
 *          which will further ensure the parity fails
 *    - whole-chunk injection will fail the connection
 *        * the 8-byte sequence number will ensure this
 *        * also the 4-byte parity check will fail for
 *          the same reasons as above
 *    - a beginning-to-end replay attack will fail the connection
 *        * because this key has never been used before
 *        * the bytes will be decrypted differently, and
 *          the sequence number and parity will fail
 *    - a replay of any byte(s) will fail the connection
 *        * <same>
 *    - byte (or chunk) re-ordering will fail the connection
 *        * same reasons: sequence number and parity checks
 *    - byte (or chunk) modification will fail the connection
 *        * same story: parity will likely fail, and all
 *          future decrypted blocks will be incorrect,
 *          making parity failure extremely likely
 *        * in the case of chunk re-ordering, the sequence
 *          number check will fail
 *    - byte (or chunk) duplication will fail the connection
 *        * <same>
 *    - byte (or chunk) deletion will fail the connection
 *        * <same>
 */
class tReadableAES : public iReadable, public bNonCopyable
{
    public:

        tReadableAES(iReadable* internalStream, nOperationModeAES opmode,
                     const u8 key[], nKeyLengthAES keylen);

        ~tReadableAES();

        i32 read(u8* buffer, i32 length);
        i32 readAll(u8* buffer, i32 length);

        void reset();

    private:

        bool m_refill();

    private:

        // Stuff:
        tBufferedReadable m_stream;
        u8* m_buf;
        u32 m_bufSize;
        u32 m_bufUsed;
        u32 m_pos;
        u64 m_seq;

        // Operation mode stuff:
        nOperationModeAES m_opmode;
        u8 m_last_ct[AES_BLOCK_SIZE];        // <-- used only in kOpModeCBC
        bool m_hasReadInitializationVector;  // <-- used only in kOpModeCBC

        // Encryption state:
        u32 m_rk[60];    // size is 4*(MAXNR+1)
        int m_Nr;
};


}    // namespace crypt
}    // namespace rho


#endif   // __rho_crypt_tReadableAES_h__
