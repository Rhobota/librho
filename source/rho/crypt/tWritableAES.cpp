#include <rho/crypt/tWritableAES.h>


#include <rho/eRho.h>
#include "rijndael-alg-fst.h"
#include <cstdlib>


namespace rho
{
namespace crypt
{


tWritableAES::tWritableAES(iWritable* internalStream,
             const u8 key[], eKeyLengthAES keylen)
    : m_stream(internalStream), m_buf(NULL),
      m_bufSize(0), m_bufUsed(0)
{
    // Setup stuff...
    if (m_stream == NULL)
        throw eNullPointer("internalStream must not be NULL.");
    m_bufSize = AES_BLOCK_SIZE*512;
    m_buf = new u8[m_bufSize];
    m_bufUsed = 4;               // <-- the first four bytes are used to store
                                 //     the size of the chunk

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
    if (m_bufUsed >= m_bufSize)
        flush();                     // <-- resets m_bufUsed to 0
    i32 i;
    for (i = 0; i < length && m_bufUsed < m_bufSize; i++)
        m_buf[m_bufUsed++] = buffer[i];
    return i;
}

i32 tWritableAES::writeAll(const u8* buffer, i32 length)
{
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

void tWritableAES::flush()
{
    if (m_bufUsed <= 4)
        return;

    // Store the size of the chunk.
    m_buf[0] = (u8)((m_bufUsed >> 24) & 0xFF);
    m_buf[1] = (u8)((m_bufUsed >> 16) & 0xFF);
    m_buf[2] = (u8)((m_bufUsed >> 8)  & 0xFF);
    m_buf[3] = (u8)((m_bufUsed >> 0)  & 0xFF);

    // Randomize the end of the last block.
    // (Removes potential predictable plain text.)
    u32 extraBytes = (m_bufUsed % AES_BLOCK_SIZE);
    u32 bytesToSend = (extraBytes > 0) ? (m_bufUsed + (AES_BLOCK_SIZE-extraBytes)) : (m_bufUsed);
    for (u32 i = m_bufUsed; i < bytesToSend; i++)
        m_buf[i] = (u8)(rand() % 256);

    // Encrypt the whole chunk.
    u8 pt[AES_BLOCK_SIZE];
    u8 ct[AES_BLOCK_SIZE];
    for (u32 i = 0; i < m_bufUsed; i += AES_BLOCK_SIZE)
    {
        for (u32 j = 0; j < AES_BLOCK_SIZE; j++)
            pt[j] = m_buf[i+j];
        rijndaelEncrypt(m_rk, m_Nr, pt, ct);
        for (u32 j = 0; j < AES_BLOCK_SIZE; j++)
            m_buf[i+j] = ct[j];
    }

    // Send the chunk.
    i32 r = m_stream->writeAll(m_buf, bytesToSend);
    if (r < 0 || ((u32)r) != bytesToSend)
    {
        // This exception could be thrown through ~tWritableAES...
        // ...so that could be bad if an exception is already in flight... :(
        throw eRuntimeError("Could not flush the AES output stream!");
    }
    m_bufUsed = 4;

    // Flush the lower buffer if it supports flushing.
    iFlushable* flushable = dynamic_cast<iFlushable*>(m_stream);
    if (flushable)
        flushable->flush();
}


}   // namespace crypt
}   // namespace rho
