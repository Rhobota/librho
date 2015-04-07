#include <rho/crypt/tEncAES.h>
#include <rho/eRho.h>

#include "aesni/iaes_asm_interface.h"
#include "rijndael-alg-fst.h"

#include <string.h>


namespace rho
{
namespace crypt
{


static
u8* s_aligned_malloc(size_t size, size_t alignment)
{
    // Inspired by: http://cottonvibes.blogspot.com/2011/01/dynamically-allocate-aligned-memory.html
    u8* r = (u8*) malloc(size + (--alignment) + sizeof(u8*));
    if (r == NULL) return NULL;
    u8* t = r + sizeof(u8*);
    u8* o = (u8*)( ( ((size_t)(t + alignment)) | alignment ) ^ alignment );
    ((u8**)(o))[-1] = r;
    return o;
}


static
void s_aligned_free(u8* buf)
{
    if (buf)
    {
        free(((u8**)(buf))[-1]);
    }
}


tEncAES::tEncAES(nOperationModeAES opmode, const u8 key[], nKeyLengthAES keylen)
{
    // See if this CPU supports the Intel AES-NI instruction set (implemented in assembly (ASM)).
    bool useFastASM = check_for_aes_instructions();

    // Call init.
    m_init(opmode, key, keylen, useFastASM);
}


tEncAES::tEncAES(nOperationModeAES opmode, const u8 key[], nKeyLengthAES keylen,
                 bool useFastASM)
{
    // Call init.
    m_init(opmode, key, keylen, useFastASM);
}


void tEncAES::enc(u8* ptbuf, u8* ctbuf, u32 numblocks, u8* iv)
{
    // Fast ASM impl:
    if (m_useASM)
    {
        sAesData data;
        data.in_block = ptbuf;
        data.out_block = ctbuf;
        data.expanded_key = m_expandedKey;
        data.iv = iv;
        data.num_blocks = numblocks;
        if (m_opmode == kOpModeCBC && iv)
        {
            switch (m_keylen)
            {
                case k128bit: iEnc128_CBC(&data); break;
                case k192bit: iEnc192_CBC(&data); break;
                case k256bit: iEnc256_CBC(&data); break;
                default: throw eInvalidArgument("The keylen parameter is not valid!");
            }
        }
        else
        {
            switch (m_keylen)
            {
                case k128bit: iEnc128(&data); break;
                case k192bit: iEnc192(&data); break;
                case k256bit: iEnc256(&data); break;
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
            for (u32 i = 0; numblocks > 0; i+=AES_BLOCK_SIZE, --numblocks)
            {
                for (u32 j = 0; j < AES_BLOCK_SIZE; j++)
                    ptbuf[i+j] ^= iv[j];
                rijndaelEncrypt(rk, Nr, ptbuf+i, ctbuf+i);
                for (u32 j = 0; j < AES_BLOCK_SIZE; j++)
                    iv[j] = ctbuf[i+j];
            }
        }
        else
        {
            for (u32 i = 0; numblocks > 0; i+=AES_BLOCK_SIZE, --numblocks)
            {
                rijndaelEncrypt(rk, Nr, ptbuf+i, ctbuf+i);
            }
        }
    }
}


tEncAES::~tEncAES()
{
    m_finalize();
}


void tEncAES::m_init(nOperationModeAES opmode, const u8 key[], nKeyLengthAES keylen,
                     bool useFastASM)
{
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

    // Set fields.
    m_opmode = opmode;
    m_keylen = keylen;
    m_useASM = useFastASM;
    m_expandedKey = NULL;

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
                iEncExpandKey128(key_copy, m_expandedKey);
                delete [] key_copy;
                break;
            }
            case k192bit:
            {
                u8* key_copy = new u8[24];
                memcpy(key_copy, key, 24);
                iEncExpandKey192(key_copy, m_expandedKey);
                delete [] key_copy;
                break;
            }
            case k256bit:
            {
                u8* key_copy = new u8[32];
                memcpy(key_copy, key, 32);
                iEncExpandKey256(key_copy, m_expandedKey);
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
        m_Nr = rijndaelKeySetupEnc(m_rk, key, keybits);
        if (m_Nr != expectedNr)
            throw eImpossiblePath();
    }
}


void tEncAES::m_finalize()
{
    s_aligned_free(m_expandedKey);
    m_expandedKey = NULL;
}


}   // namespace crypt
}   // namespace rho
