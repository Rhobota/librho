#include <rho/crypt/tMD5.h>

#include "md5.h"

#include <sstream>
#include <iomanip>


namespace rho
{
namespace crypt
{


tMD5::tMD5()
    : m_context(NULL)
{
    m_context = new MD5_CTX;
    MD5Init((MD5_CTX*)m_context);
}

tMD5::~tMD5()
{
    delete ((MD5_CTX*)m_context);
    m_context = NULL;
}

i32 tMD5::write(const u8* buffer, i32 length)
{
    return this->writeAll(buffer, length);
}

i32 tMD5::writeAll(const u8* buffer, i32 length)
{
    MD5Update(((MD5_CTX*)m_context), buffer, (u32)length);
    return length;
}

std::vector<u8> tMD5::getHash() const
{
    std::vector<u8> v(16, 0);
    MD5_CTX context = *((MD5_CTX*)m_context);
    MD5Final(&(v[0]), &context);
    return v;
}

std::string tMD5::getHashString() const
{
    std::vector<u8> hash = getHash();
    std::ostringstream out;
    for (u32 i = 0; i < hash.size(); i++)
        out << std::hex << std::setfill('0') << std::setw(2) << (u32)hash[i];
    return out.str();
}


}   // namespace crypt
}   // namespace rho
