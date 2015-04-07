#ifndef __rho_crypt_tDecAES_h__
#define __rho_crypt_tDecAES_h__


#include <rho/bNonCopyable.h>
#include <rho/types.h>
#include <rho/crypt/nOperationModeAES.h>
#include <rho/crypt/nKeyLength.h>

#ifndef AES_BLOCK_SIZE
#define AES_BLOCK_SIZE 16
#endif


namespace rho
{
namespace crypt
{


class tDecAES : public bNonCopyable
{
    public:

        /**
         * The length of the 'key' buffer should match the expected length of
         * a key described by 'keylen':
         *    - if keylen == k128bit, then the 'key' buffer should be 16 bytes
         *    - if keylen == k192bit, then the 'key' buffer should be 24 bytes
         *    - if keylen == k256bit, then the 'key' buffer should be 32 bytes
         */
        tDecAES(nOperationModeAES opmode, const u8 key[], nKeyLengthAES keylen);

        /**
         * Decrypts cypher text data (in the 'ctbuf' buffer) and places
         * it into the plain text buffer ('ptbuf').
         *
         * Both 'ctbuf' and 'ptbuf' should be the same size, and that size
         * must be a multiple of 16 bytes (because a "block" in AES is 16 bytes).
         * The number of 16-byte-blocks that you want processed should be passed
         * as 'numblocks'.
         *
         * Pass an 'iv' vector if you constructed this object with:
         *   opmode == kOpModeCBC
         * The 'iv' vector, if passed, must be 16 bytes.
         */
        void dec(u8* ctbuf, u8* ptbuf, u32 numblocks, u8* iv=NULL);

    private:

        nOperationModeAES m_opmode;
        nKeyLengthAES m_keylen;

        // State used by the fallback implementation:
        u32 m_rk[60];    // size is 4*(MAXNR+1)
        int m_Nr;
};


}  // namespace crypt
}  // namespace rho


#endif   // __rho_crypt_tDecAES_h__