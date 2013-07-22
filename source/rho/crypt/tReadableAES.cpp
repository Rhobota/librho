#include <rho/crypt/tReadableAES.h>
#include <rho/eRho.h>

#include "rijndael-alg-fst.h"

#include <algorithm>
#include <cstdlib>


namespace rho
{
namespace crypt
{


tReadableAES::tReadableAES(iReadable* internalStream, nOperationModeAES opmode,
             const u8 key[], nKeyLengthAES keylen)
    : m_stream(internalStream), m_buf(NULL),
      m_bufSize(0), m_bufUsed(0), m_pos(0),
      m_chunkBytesLeftToRead(0),
      m_opmode(opmode)
{
    // Setup stuff...
    m_bufSize = AES_BLOCK_SIZE;
    m_buf = new u8[m_bufSize];

    // Setup encryption state...
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

    // Check the op mode.
    switch (m_opmode)
    {
        case kOpModeECB:
            break;

        case kOpModeCBC:
            m_hasReadInitializationVector = false;
            break;

        default: throw eInvalidArgument("opmode is not recognized!");
    }
}

tReadableAES::~tReadableAES()
{
    delete [] m_buf;
    m_buf = NULL;
    m_bufSize = 0;
    m_bufUsed = 0;
    m_pos = 0;
    m_chunkBytesLeftToRead = 0;
}

i32 tReadableAES::read(u8* buffer, i32 length)
{
    if (m_pos >= m_bufUsed)
        if (! refill())      // sets m_pos and m_bufUsed
            return -1;
    i32 i;
    for (i = 0; i < length && m_pos < m_bufUsed; i++)
        buffer[i] = m_buf[m_pos++];
    return i;
}

i32 tReadableAES::readAll(u8* buffer, i32 length)
{
    i32 amountRead = 0;
    while (amountRead < length)
    {
        i32 n = read(buffer+amountRead, length-amountRead);
        if (n <= 0)
            return (amountRead>0) ? amountRead : n;
        amountRead += n;
    }
    return amountRead;
}

bool tReadableAES::refill()
{
    // If in cbc mode and this is the first time refill is called, read the
    // initialization vector off the stream and decrypt it.
    if (m_opmode == kOpModeCBC && !m_hasReadInitializationVector)
    {
        u8 initVectorCt[AES_BLOCK_SIZE];
        i32 r = m_stream.readAll(initVectorCt, AES_BLOCK_SIZE);
        if (r != AES_BLOCK_SIZE)
        {
            // throw eRuntimeError("The AES reader could not read the CBC "
            //       "initialization vector!");
            return false;
        }
        rijndaelDecrypt(m_rk, m_Nr, initVectorCt, m_last_ct);
        m_hasReadInitializationVector = true;
    }

    // Reset indices.
    m_pos = 0;
    m_bufUsed = 0;

    // Read an AES block from the stream.
    u8  ct[AES_BLOCK_SIZE];
    i32 r = m_stream.readAll(ct, AES_BLOCK_SIZE);
    if (r != AES_BLOCK_SIZE)
        return false;

    // Decrypt the ct buffer into the pt buffer.
    u8 pt[AES_BLOCK_SIZE];
    rijndaelDecrypt(m_rk, m_Nr, ct, pt);

    // If in CBC mode, deal with the xor chaining stuff.
    if (m_opmode == kOpModeCBC)
    {
        for (int i = 0; i < AES_BLOCK_SIZE; i++)
        {
            pt[i] ^= m_last_ct[i];
            m_last_ct[i] = ct[i];
        }
    }

    // Keep track of where in the chunk we are at.
    if (m_chunkBytesLeftToRead == 0)
    {
        m_chunkBytesLeftToRead =
            (pt[0] << 24) |
            (pt[1] << 16) |
            (pt[2] <<  8) |
            (pt[3] <<  0);
        if (m_chunkBytesLeftToRead <= 4)
            throw eImpossiblePath();
        m_bufUsed = std::min((u32)AES_BLOCK_SIZE, m_chunkBytesLeftToRead) - 4;
        for (u32 i = 0; i < m_bufUsed; i++)
            m_buf[i] = pt[i+4];
        m_chunkBytesLeftToRead -= 4;
        m_chunkBytesLeftToRead -= m_bufUsed;
    }
    else
    {
        m_bufUsed = std::min((u32)AES_BLOCK_SIZE, m_chunkBytesLeftToRead);
        for (u32 i = 0; i < m_bufUsed; i++)
            m_buf[i] = pt[i];
        m_chunkBytesLeftToRead -= m_bufUsed;
    }

    return true;
}

void tReadableAES::reset()
{
    m_pos = 0;
    m_bufUsed = 0;
    m_chunkBytesLeftToRead = 0;
}


}    // namespace crypt
}    // namespace rho
