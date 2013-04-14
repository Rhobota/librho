#include <rho/crypt/tReadableAES.h>


#include "rijndael-alg-fst.h"
#define AES_BLOCK_SIZE 16

#include <rho/eRho.h>


namespace rho
{
namespace crypt
{


tReadableAES::tReadableAES(iReadable* internalStream,
             const u8 key[], eKeyLengthAES keylen)
    : m_stream(internalStream), m_buf(NULL),
      m_bufSize(0), m_bufUsed(0), m_pos(0),
      m_chunkBytesLeftToRead(0)
{
    // Setup stuff...
    m_bufSize = AES_BLOCK_SIZE;
    m_buf = new u8[m_bufSize];

    // Setup encryption state...
    int keybits;
    switch (keylen)
    {
        case k128bit: keybits = 128; break;
        case k192bit: keybits = 192; break;
        case k256bit: keybits = 256; break;
        default: throw eInvalidArgument("The keylen parameter is not valid!");
    }
    m_Nr = rijndaelKeySetupDec(m_rk, key, keybits);
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
    // Reset indices.
    m_pos = 0;
    m_bufUsed = 0;

    // Read a block or a block-plus-4-bytes into a temporary buffer.
    i32 needToRead = (m_chunkBytesLeftToRead > 0) ? AES_BLOCK_SIZE : AES_BLOCK_SIZE+4;
    u8  tempbuf[AES_BLOCK_SIZE+4];
    i32 r = m_stream.readAll(tempbuf, needToRead);

    // Ensure we read as much as we were supposed to read.
    if (r != needToRead)
        return false;

    // Keep track of where in the chunk we are at, and copy the ct to another buffer.
    u8 ct[AES_BLOCK_SIZE];
    if (m_chunkBytesLeftToRead > 0)
    {
        m_chunkBytesLeftToRead -= needToRead;
        for (int i = 0; i < AES_BLOCK_SIZE; i++)
            ct[i] = tempbuf[i];
    }
    else
    {
        m_chunkBytesLeftToRead =
            (tempbuf[0] << 24) |
            (tempbuf[1] << 16) |
            (tempbuf[2] <<  8) |
            (tempbuf[3] <<  0);
        if (m_chunkBytesLeftToRead < ((u32)needToRead) ||
              (m_chunkBytesLeftToRead % AES_BLOCK_SIZE) != 4)
        {
            throw eImpossiblePath();
        }

        m_chunkBytesLeftToRead -= needToRead;

        if ((m_chunkBytesLeftToRead % AES_BLOCK_SIZE) > 0)
        {
            throw eImpossiblePath();
        }

        for (int i = 0; i < AES_BLOCK_SIZE; i++)
            ct[i] = tempbuf[i+4];
    }

    // Decrypt the ct buffer into the pt buffer.
    u8 pt[AES_BLOCK_SIZE];
    rijndaelDecrypt(m_rk, m_Nr, ct, pt);
    for (int i = 0; i < AES_BLOCK_SIZE; i++)
        m_buf[i] = pt[i];
    m_bufUsed = AES_BLOCK_SIZE;

    return true;
}


}    // namespace crypt
}    // namespace rho
