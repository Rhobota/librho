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
    : m_stream(internalStream),
      m_buf(NULL), m_bufSize(0),
      m_bufUsed(0), m_pos(0),
      m_seq(0),
      m_opmode(opmode)
{
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

    // Alloc the chunk buffer.
    m_bufSize = 512*AES_BLOCK_SIZE;
    m_buf = new u8[m_bufSize];
}

tReadableAES::~tReadableAES()
{
    delete [] m_buf;
    m_buf = NULL;
    m_bufSize = 0;
    m_bufUsed = 0;
    m_pos = 0;
}

i32 tReadableAES::read(u8* buffer, i32 length)
{
    if (length <= 0)
        throw eInvalidArgument("Stream read/write length must be >0");

    if (m_pos >= m_bufUsed)
        if (! m_refill())      // sets m_pos and m_bufUsed
            return -1;

    u32 rem = m_bufUsed - m_pos;
    if (rem > (u32)length)
        rem = (u32)length;

    memcpy(buffer, m_buf, rem);
    m_pos += rem;

    return (i32)rem;
}

i32 tReadableAES::readAll(u8* buffer, i32 length)
{
    if (length <= 0)
        throw eInvalidArgument("Stream read/write length must be >0");

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

bool tReadableAES::m_refill()
{
    // Reset indices.
    m_pos = 0;
    m_bufUsed = 0;

    // If in cbc mode and this is the first time m_refill is called, read the
    // initialization vector off the stream and decrypt it.
    if (m_opmode == kOpModeCBC && !m_hasReadInitializationVector)
    {
        u8 initVectorCt[AES_BLOCK_SIZE];
        i32 r = m_stream.readAll(initVectorCt, AES_BLOCK_SIZE);
        if (r != AES_BLOCK_SIZE)
        {
            return (r >= 0);  // <-- makes read() give the expected behavior
        }
        rijndaelDecrypt(m_rk, m_Nr, initVectorCt, m_last_ct);
        m_hasReadInitializationVector = true;
    }

    // Read an AES block from the stream.
    u8  ct[AES_BLOCK_SIZE];
    i32 r = m_stream.readAll(ct, AES_BLOCK_SIZE);
    if (r != AES_BLOCK_SIZE)
    {
        return (r >= 0);  // <-- makes read() give the expected behavior
    }

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

    // We just read the first block (16 bytes) of a chunk from the stream.
    // This block contains the chunk's length, sequence number, and parity.

    // Extract the chunk length.
    u32 chunkLen = 0;
    chunkLen |= pt[0]; chunkLen <<= 8;
    chunkLen |= pt[1]; chunkLen <<= 8;
    chunkLen |= pt[2]; chunkLen <<= 8;
    chunkLen |= pt[3];

    // Extract the sequence number.
    u64 seq = 0;
    seq |= pt[ 4]; seq <<= 8;
    seq |= pt[ 5]; seq <<= 8;
    seq |= pt[ 6]; seq <<= 8;
    seq |= pt[ 7]; seq <<= 8;
    seq |= pt[ 8]; seq <<= 8;
    seq |= pt[ 9]; seq <<= 8;
    seq |= pt[10]; seq <<= 8;
    seq |= pt[11];

    // Verify that these values are correct.
    if (chunkLen <= 16)
        throw eRuntimeError("This stream is not a valid AES stream. The chunk length is <=16.");
    if (chunkLen > m_bufSize)
        throw eRuntimeError("This stream is not a valid AES stream. The chunk length is too large.");
    if (seq != m_seq)
        throw eRuntimeError("This stream is not a valid AES stream. The sequence number is not what was expected.");
    m_seq += chunkLen;

    // Start the parity check.
    u8 parity[4] = { 0, 0, 0, 0 };
    for (u32 i = 0; i < AES_BLOCK_SIZE; i++)
        parity[i%4] ^= pt[i];

    // Read the rest of the chunk into our buffer.
    chunkLen -= AES_BLOCK_SIZE;
    u32 extraBytes = (chunkLen % AES_BLOCK_SIZE);
    u32 bytesToRead = (extraBytes > 0) ? (chunkLen + (AES_BLOCK_SIZE-extraBytes)) : (chunkLen);
    r = m_stream.readAll(m_buf, bytesToRead);
    if (r < 0 || ((u32)r) != bytesToRead)
    {
        return (r >= 0);  // <-- makes read() give the expected behavior
    }

    // Decrypt the bytes we just read.
    for (u32 i = 0; i < bytesToRead; i += AES_BLOCK_SIZE)
    {
        // Decrypt this block.
        for (u32 j = 0; j < AES_BLOCK_SIZE; j++)
            ct[j] = m_buf[i+j];
        rijndaelDecrypt(m_rk, m_Nr, ct, m_buf+i);

        // If in CBC mode, deal with the xor chaining stuff.
        if (m_opmode == kOpModeCBC)
        {
            for (int j = 0; j < AES_BLOCK_SIZE; j++)
            {
                m_buf[i+j] ^= m_last_ct[j];
                parity[(i+j)%4] ^= m_buf[i+j];
                m_last_ct[j] = ct[j];
            }
        }
        else
        {
            for (int j = 0; j < AES_BLOCK_SIZE; j++)
            {
                parity[(i+j)%4] ^= m_buf[i+j];
            }
        }
    }

    // Make sure the parity works out.
    for (u32 i = chunkLen; i < bytesToRead; i++)
        parity[i%4] ^= m_buf[i];
    for (u32 i = 0; i < 4; i++)
        if (parity[i] != 0)
            throw eRuntimeError("This stream is not a valid AES stream. The parity is broken.");

    // All must have worked, so set the state up for reading.
    m_bufUsed = chunkLen;

    return true;
}

void tReadableAES::reset()
{
    m_pos = 0;
    m_bufUsed = 0;
    m_seq = 0;
    if (m_opmode == kOpModeCBC)
        m_hasReadInitializationVector = false;
}


}    // namespace crypt
}    // namespace rho
