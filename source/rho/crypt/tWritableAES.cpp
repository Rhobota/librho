#include <rho/crypt/tWritableAES.h>
#include <rho/crypt/tSecureRandom.h>

#include <rho/eRho.h>
#include "rijndael-alg-fst.h"
#include <cstdlib>


namespace rho
{
namespace crypt
{


tWritableAES::tWritableAES(iWritable* internalStream, nOperationModeAES opmode,
             const u8 key[], nKeyLengthAES keylen)
    : m_stream(internalStream), m_buf(NULL),
      m_bufSize(0), m_bufUsed(0), m_seq(0),
      m_opmode(opmode)
{
    if (m_stream == NULL)
        throw eNullPointer("internalStream must not be NULL.");

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
    m_Nr = rijndaelKeySetupEnc(m_rk, key, keybits);
    if (m_Nr != expectedNr)
        throw eImpossiblePath();

    // Check the op mode.
    switch (m_opmode)
    {
        case kOpModeECB:
            break;

        case kOpModeCBC:
            // Randomize the initialization vector
            secureRand_readAll(m_last_ct, AES_BLOCK_SIZE);
            m_hasSentInitializationVector = false;
            break;

        default: throw eInvalidArgument("opmode is not recognized!");
    }

    // Alloc stuff...
    m_bufSize = AES_BLOCK_SIZE*512;
    m_buf = new u8[m_bufSize];
    m_bufUsed = 16;               // <-- the first 16 bytes are used to store:
                                  //       1. the size of the chunk (4 bytes)
                                  //       2. the sequence number of this chunk (8 bytes)
                                  //       3. the parity of this chunk (4 bytes)
}

tWritableAES::~tWritableAES()
{
    flush();
    m_stream = NULL;
    delete [] m_buf;
    m_buf = NULL;
    m_bufSize = 0;
    m_bufUsed = 0;
}

i32 tWritableAES::write(const u8* buffer, i32 length)
{
    if (length <= 0)
        throw eInvalidArgument("Stream read/write length must be >0");

    if (m_bufUsed >= m_bufSize)
        if (! flush())    // <-- if successful, resets m_bufUsed to 16
            return 0;
    i32 i;
    for (i = 0; i < length && m_bufUsed < m_bufSize; i++)
        m_buf[m_bufUsed++] = buffer[i];
    return i;
}

i32 tWritableAES::writeAll(const u8* buffer, i32 length)
{
    if (length <= 0)
        throw eInvalidArgument("Stream read/write length must be >0");

    i32 amountWritten = 0;
    while (amountWritten < length)
    {
        i32 n = write(buffer+amountWritten, length-amountWritten);
        if (n <= 0)
            return (amountWritten>0) ? amountWritten : n;
        amountWritten += n;
    }
    return amountWritten;
}

bool tWritableAES::flush()
{
    if (m_bufUsed <= 16)
        return true;

    // If in cbc mode and this is the first flush, send the initialization
    // vector to the reader.
    if (m_opmode == kOpModeCBC && !m_hasSentInitializationVector)
    {
        u8 initVectorCt[AES_BLOCK_SIZE];
        rijndaelEncrypt(m_rk, m_Nr, m_last_ct, initVectorCt);
        i32 w = m_stream->writeAll(initVectorCt, AES_BLOCK_SIZE);
        if (w != AES_BLOCK_SIZE)
            return false;
        m_hasSentInitializationVector = true;
    }

    // Store the size of the chunk.
    m_buf[0] = (u8)((m_bufUsed >> 24) & 0xFF);
    m_buf[1] = (u8)((m_bufUsed >> 16) & 0xFF);
    m_buf[2] = (u8)((m_bufUsed >>  8) & 0xFF);
    m_buf[3] = (u8)((m_bufUsed      ) & 0xFF);

    // Store the sequence number of this chunk.
    m_buf[ 4] = (u8)((m_seq >> 56) & 0xFF);
    m_buf[ 5] = (u8)((m_seq >> 48) & 0xFF);
    m_buf[ 6] = (u8)((m_seq >> 40) & 0xFF);
    m_buf[ 7] = (u8)((m_seq >> 32) & 0xFF);
    m_buf[ 8] = (u8)((m_seq >> 24) & 0xFF);
    m_buf[ 9] = (u8)((m_seq >> 16) & 0xFF);
    m_buf[10] = (u8)((m_seq >>  8) & 0xFF);
    m_buf[11] = (u8)((m_seq      ) & 0xFF);
    m_seq += m_bufUsed;

    // Store the parity of this chunk.
    u8 parity[4] = { 0, 0, 0, 0 };
    for (u32 i = 0; i < 12; i++)         parity[i%4] ^= m_buf[i];
    for (u32 i = 16; i < m_bufUsed; i++) parity[i%4] ^= m_buf[i];
    m_buf[12] = parity[0];
    m_buf[13] = parity[1];
    m_buf[14] = parity[2];
    m_buf[15] = parity[3];

    // Randomize the end of the last block.
    // (Removes potential predictable plain text.)
    u32 extraBytes = (m_bufUsed % AES_BLOCK_SIZE);
    u32 bytesToSend = (extraBytes > 0) ? (m_bufUsed + (AES_BLOCK_SIZE-extraBytes)) : (m_bufUsed);
    if (bytesToSend > m_bufUsed)
        secureRand_readAll(m_buf+m_bufUsed, bytesToSend-m_bufUsed);

    // Encrypt the whole chunk.
    u8 pt[AES_BLOCK_SIZE];
    u8 ct[AES_BLOCK_SIZE];
    for (u32 i = 0; i < m_bufUsed; i += AES_BLOCK_SIZE)
    {
        for (u32 j = 0; j < AES_BLOCK_SIZE; j++)
            pt[j] = m_buf[i+j];

        if (m_opmode == kOpModeCBC)
            for (u32 j = 0; j < AES_BLOCK_SIZE; j++)
                pt[j] ^= m_last_ct[j];

        rijndaelEncrypt(m_rk, m_Nr, pt, ct);

        for (u32 j = 0; j < AES_BLOCK_SIZE; j++)
            m_buf[i+j] = ct[j];

        if (m_opmode == kOpModeCBC)
            for (u32 j = 0; j < AES_BLOCK_SIZE; j++)
                m_last_ct[j] = ct[j];
    }

    // Send the chunk.
    i32 r = m_stream->writeAll(m_buf, bytesToSend);
    if (r < 0 || ((u32)r) != bytesToSend)
        return false;
    m_bufUsed = 16;

    // Flush the lower buffer if it supports flushing.
    iFlushable* flushable = dynamic_cast<iFlushable*>(m_stream);
    if (flushable)
        return flushable->flush();
    else
        return true;
}

void tWritableAES::reset()
{
    m_bufUsed = 16;
    m_seq = 0;
    if (m_opmode == kOpModeCBC)
        m_hasSentInitializationVector = false;
}


}   // namespace crypt
}   // namespace rho
