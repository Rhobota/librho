#include <rho/crypt/tDecAES.h>
#include <rho/eRho.h>

#include "rijndael-alg-fst.h"


namespace rho
{
namespace crypt
{


tDecAES::tDecAES(nOperationModeAES opmode, const u8 key[], nKeyLengthAES keylen)
    : m_opmode(opmode), m_keylen(keylen)
{
    // Check the opmode.
    switch (opmode)
    {
        case kOpModeECB: break;
        case kOpModeCBC: break;
        default: throw eInvalidArgument("The opmode parameter is not valid!");
    }

    // Fallback setup:
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


void tDecAES::dec(u8* ctbuf, u8* ptbuf, u32 numblocks, u8* iv)
{
    // Fallback impl:
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


}  // namespace crypt
}  // namespace rho
