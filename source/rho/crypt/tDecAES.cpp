#include <rho/crypt/tDecAES.h>
#include <rho/eRho.h>

#include "aesni/iaes_asm_interface.h"
#include "rijndael-alg-fst.h"

#if __MINGW32__
#include <malloc.h>
#endif
#include <string.h>


namespace rho
{
namespace crypt
{


#include "alignedmem.ipp"


tDecAES::tDecAES(nOperationModeAES opmode, const u8 key[], nKeyLengthAES keylen)
{
    // See if this CPU supports the Intel AES-NI instruction set (implemented in assembly (ASM)).
    bool useFastASM = check_for_aes_instructions();

    // Call init.
    m_init(opmode, key, keylen, useFastASM);
}


tDecAES::tDecAES(nOperationModeAES opmode, const u8 key[], nKeyLengthAES keylen,
                 bool useFastASM)
{
    // Call init.
    m_init(opmode, key, keylen, useFastASM);
}


void tDecAES::dec(u8* ctbuf, u8* ptbuf, u32 numblocks, u8* iv)
{
    // Fast ASM impl:
    if (m_useASM)
    {
        sAesData data;
        data.in_block = ctbuf;
        data.out_block = ptbuf;
        data.expanded_key = m_expandedKey;
        data.iv = iv;
        data.num_blocks = numblocks;
        if (m_opmode == kOpModeCBC && iv)
        {
            switch (m_keylen)
            {
                case k128bit: iDec128_CBC(&data); break;
                case k192bit: iDec192_CBC(&data); break;
                case k256bit: iDec256_CBC(&data); break;
                default: throw eInvalidArgument("The keylen parameter is not valid!");
            }
        }
        else
        {
            switch (m_keylen)
            {
                case k128bit: iDec128(&data); break;
                case k192bit: iDec192(&data); break;
                case k256bit: iDec256(&data); break;
                default: throw eInvalidArgument("The keylen parameter is not valid!");
            }
        }
    }

    // Fallback impl:
    else
    {
        u32* rk = m_rk;
        int Nr = m_Nr;
        if (m_opmode == kOpModeCBC && iv)
        {
            u8 ct[AES_BLOCK_SIZE];
            for (u32 i = 0; numblocks > 0; i+=AES_BLOCK_SIZE, --numblocks)
            {
                for (u32 j = 0; j < AES_BLOCK_SIZE; j++)
                    ct[j] = ctbuf[i+j];
                rijndaelDecrypt(rk, Nr, ct, ptbuf+i);
                for (u32 j = 0; j < AES_BLOCK_SIZE; j++)
                {
                    ptbuf[i+j] ^= iv[j];
                    iv[j] = ct[j];
                }
            }
        }
        else
        {
            for (u32 i = 0; numblocks > 0; i+=AES_BLOCK_SIZE, --numblocks)
            {
                rijndaelDecrypt(rk, Nr, ctbuf+i, ptbuf+i);
            }
        }
    }
}


tDecAES::~tDecAES()
{
    m_finalize();
}


bool tDecAES::canRunFastASM()
{
    return check_for_aes_instructions();
}


void tDecAES::m_init(nOperationModeAES opmode, const u8 key[], nKeyLengthAES keylen,
                     bool useFastASM)
{
    // Set fields.
    m_opmode = opmode;
    m_keylen = keylen;
    m_useASM = useFastASM;
    m_expandedKey = NULL;

    // Check the opmode.
    switch (opmode)
    {
        case kOpModeECB: break;
        case kOpModeCBC: break;
        default: throw eInvalidArgument("The opmode parameter is not valid!");
    }

    // Check the keylen.
    switch (keylen)
    {
        case k128bit: break;
        case k192bit: break;
        case k256bit: break;
        default: throw eInvalidArgument("The keylen parameter is not valid!");
    }

    // Fast ASM setup:
    if (m_useASM)
    {
        m_expandedKey = s_aligned_malloc(256, 16);
        switch (keylen)
        {
            case k128bit:
            {
                u8* key_copy = new u8[16];
                memcpy(key_copy, key, 16);
                iDecExpandKey128(key_copy, m_expandedKey);
                delete [] key_copy;
                break;
            }
            case k192bit:
            {
                u8* key_copy = new u8[24];
                memcpy(key_copy, key, 24);
                iDecExpandKey192(key_copy, m_expandedKey);
                delete [] key_copy;
                break;
            }
            case k256bit:
            {
                u8* key_copy = new u8[32];
                memcpy(key_copy, key, 32);
                iDecExpandKey256(key_copy, m_expandedKey);
                delete [] key_copy;
                break;
            }
            default: throw eInvalidArgument("The keylen parameter is not valid!");
        }
    }

    // Fallback setup:
    else
    {
        int keybits;
        int expectedNr;
        switch (keylen)
        {
            case k128bit: keybits = 128; expectedNr = 10; break;
            case k192bit: keybits = 192; expectedNr = 12; break;
            case k256bit: keybits = 256; expectedNr = 14; break;
            default: throw eInvalidArgument("The keylen parameter is not valid!");
        }
        m_Nr = rijndaelKeySetupDec(m_rk, key, keybits);
        if (m_Nr != expectedNr)
            throw eImpossiblePath();
    }
}


void tDecAES::m_finalize()
{
    s_aligned_free(m_expandedKey);
    m_expandedKey = NULL;
}


}  // namespace crypt
}  // namespace rho
