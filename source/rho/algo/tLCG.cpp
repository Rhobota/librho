#include <rho/algo/tLCG.h>


namespace rho
{
namespace algo
{


tLCG::tLCG(u32 seed)
    : m_x(seed)
{
}

i32 tLCG::read(u8* buffer, i32 length)
{
    return readAll(buffer, length);
}

i32 tLCG::readAll(u8* buffer, i32 length)
{
    if (length <= 0)
        throw eInvalidArgument("Stream read/write length must be >0");

    for (i32 i = 0; i < length; i++)
        buffer[i] = (u8)(next() & 0xFF);

    return length;
}

u32 tLCG::next()
{
    const u32 a = 1103515245;
    const u32 c = 12345;
    m_x = (a*m_x + c) & 0x7FFFFFFF;
    return m_x;
}

u32 tLCG::randMax()
{
    return 0x7FFFFFFF;
}


tKnuthLCG::tKnuthLCG(u64 seed)
    : m_x(seed), m_curr(0), m_currLeft(0)
{
}

i32 tKnuthLCG::read(u8* buffer, i32 length)
{
    return readAll(buffer, length);
}

i32 tKnuthLCG::readAll(u8* buffer, i32 length)
{
    if (length <= 0)
        throw eInvalidArgument("Stream read/write length must be >0");

    for (i32 i = 0; i < length; i++)
    {
        if (m_currLeft == 0)
        {
            m_curr = next();
            m_currLeft = 8;
        }

        buffer[i] = (u8)(m_curr & 0xFF);
        m_curr >>= 8;
        m_currLeft--;
    }

    return length;
}

u64 tKnuthLCG::next()
{
    const u64 a = 6364136223846793005LL;
    const u64 c = 1442695040888963407LL;
    m_x = a*m_x + c;
    return m_x;
}

u64 tKnuthLCG::randMax()
{
    return 0xFFFFFFFFFFFFFFFF;
}


tLCG gDefaultLCG;
tKnuthLCG gKnuthLCG;


}   // namespace algo
}   // namespace rho
