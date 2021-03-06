#include <rho/crypt/tWritableAES.h>
#include <rho/crypt/tSecureRandom.h>

#include <rho/eRho.h>
#include <cstdlib>


namespace rho
{
namespace crypt
{


tWritableAES::tWritableAES(iWritable* internalStream, nOperationModeAES opmode,
             const u8 key[], nKeyLengthAES keylen)
    : m_stream(internalStream), m_buf(NULL),
      m_bufSize(0), m_bufUsed(0), m_seq(0),
      m_opmode(opmode), m_aes(opmode, key, keylen)
{
    if (m_stream == NULL)
        throw eNullPointer("internalStream must not be NULL.");

    // Check the op mode.
    if (m_opmode == kOpModeCBC)
    {
        // Randomize the initialization vector
        secureRand_readAll(m_last_ct, AES_BLOCK_SIZE);
        m_hasSentInitializationVector = false;
    }

    // Alloc stuff...
    m_bufSize = AES_BLOCK_SIZE*512;
    m_buf = new u8[m_bufSize];
    m_bufUsed = 16;               // <-- the first 16 bytes are used to store:
                                  //       1. the size of the chunk (4 bytes)
                                  //       2. the sequence number of this chunk (8 bytes)
                                  //       3. the parity of this chunk (4 bytes)
    for (int i = 0; i < 4; i++)
        m_parity[i] = 0;
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

    u32 rem = m_bufSize - m_bufUsed;
    if (rem > (u32)length)
        rem = (u32)length;

    u8* parity = m_parity;
    u8* buf = m_buf;
    u32 bufUsed = m_bufUsed;

    for (u32 i = 0; i < rem; i++)
    {
        parity[bufUsed%4] ^= buffer[i];
        buf[bufUsed++] = buffer[i];
    }

    m_bufUsed = bufUsed;

    return (i32)rem;
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
        m_aes.enc(m_last_ct, initVectorCt, 1);
        i32 w = m_stream->writeAll(initVectorCt, AES_BLOCK_SIZE);
        if (w != AES_BLOCK_SIZE)
            return false;
        m_hasSentInitializationVector = true;
    }

    // Pointer aliasing.
    u8* buf = m_buf;

    // Store the size of the chunk.
    buf[0] = (u8)((m_bufUsed >> 24) & 0xFF);
    buf[1] = (u8)((m_bufUsed >> 16) & 0xFF);
    buf[2] = (u8)((m_bufUsed >>  8) & 0xFF);
    buf[3] = (u8)((m_bufUsed      ) & 0xFF);

    // Store the sequence number of this chunk.
    buf[ 4] = (u8)((m_seq >> 56) & 0xFF);
    buf[ 5] = (u8)((m_seq >> 48) & 0xFF);
    buf[ 6] = (u8)((m_seq >> 40) & 0xFF);
    buf[ 7] = (u8)((m_seq >> 32) & 0xFF);
    buf[ 8] = (u8)((m_seq >> 24) & 0xFF);
    buf[ 9] = (u8)((m_seq >> 16) & 0xFF);
    buf[10] = (u8)((m_seq >>  8) & 0xFF);
    buf[11] = (u8)((m_seq      ) & 0xFF);
    m_seq += m_bufUsed;

    // Store the parity of this chunk.
    for (u32 i = 0; i < 12; i++)
        m_parity[i%4] ^= buf[i];
    buf[12] = m_parity[0]; m_parity[0] = 0;
    buf[13] = m_parity[1]; m_parity[1] = 0;
    buf[14] = m_parity[2]; m_parity[2] = 0;
    buf[15] = m_parity[3]; m_parity[3] = 0;

    // Randomize the end of the last block.
    // (Removes potential predictable plain text.)
    u32 extraBytes = (m_bufUsed % AES_BLOCK_SIZE);
    u32 bytesToSend = (extraBytes > 0) ? (m_bufUsed + (AES_BLOCK_SIZE-extraBytes)) : (m_bufUsed);
    if (bytesToSend > m_bufUsed)
        secureRand_readAll(buf+m_bufUsed, bytesToSend-m_bufUsed);

    // Encrypt the whole chunk.
    m_aes.enc(buf, buf, bytesToSend/AES_BLOCK_SIZE, m_last_ct);

    // Send the chunk.
    i32 r = m_stream->writeAll(buf, bytesToSend);
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
