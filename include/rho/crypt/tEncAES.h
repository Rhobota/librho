#ifndef __rho_crypt_tEncAES_h__
#define __rho_crypt_tEncAES_h__


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


class tEncAES : public bNonCopyable
{
    public:

        /**
         * The length of the 'key' buffer should match the expected length of
         * a key described by 'keylen':
         *    - if keylen == k128bit, then the 'key' buffer should be 16 bytes
         *    - if keylen == k192bit, then the 'key' buffer should be 24 bytes
         *    - if keylen == k256bit, then the 'key' buffer should be 32 bytes
         *
         * This c'tor automagically detects if the fast ASM impl can be used,
         * and if so it uses it.
         */
        tEncAES(nOperationModeAES opmode, const u8 key[], nKeyLengthAES keylen);

        /**
         * Same as the c'tor above, but lets the caller explicitly decide whether
         * the fast ASM impl is used or whether the fallback impl is used.
         */
        tEncAES(nOperationModeAES opmode, const u8 key[], nKeyLengthAES keylen,
                bool useFastASM);

        /**
         * Encrypts plain text data (in the 'ptbuf' buffer) and places
         * it into the cypher text buffer ('ctbuf').
         *
         * Both 'ptbuf' and 'ctbuf' should be the same size, and that size
         * must be a multiple of 16 bytes (because a "block" in AES is 16 bytes).
         * The number of 16-byte-blocks that you want processed should be passed
         * as 'numblocks'.
         *
         * Pass an 'iv' vector if you constructed this object with:
         *   opmode == kOpModeCBC
         * The 'iv' vector, if passed, must be 16 bytes.
         */
        void enc(u8* ptbuf, u8* ctbuf, u32 numblocks, u8* iv=NULL);

        /**
         * Free stuff.
         */
        ~tEncAES();

        /**
         * Tells whether the fast ASM impl will run on this machine.
         */
        static bool canRunFastASM();

    private:

        void m_init(nOperationModeAES opmode, const u8 key[], nKeyLengthAES keylen,
                    bool useFastASM);

        void m_finalize();

    private:

        nOperationModeAES m_opmode;
        nKeyLengthAES m_keylen;
        bool m_useASM;

        // State used by the fast ASM implementation:
        u8* m_expandedKey;

        // State used by the fallback implementation:
        u32 m_rk[60];    // size is 4*(MAXNR+1)
        int m_Nr;
};


}  // namespace crypt
}  // namespace rho


#endif   // __rho_crypt_tEncAES_h__
