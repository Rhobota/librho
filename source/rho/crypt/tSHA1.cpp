#include <rho/crypt/tSHA1.h>
#include <rho/crypt/tMD5.h>

#include "sha.h"


namespace rho
{
namespace crypt
{


tSHA1::tSHA1()
    : m_context(NULL)
{
    m_context = new SHA_CTX;
    SHA1_Init((SHA_CTX*)m_context);
}

tSHA1::~tSHA1()
{
    delete ((SHA_CTX*)m_context);
    m_context = NULL;
}

i32 tSHA1::write(const u8* buffer, i32 length)
{
    return this->writeAll(buffer, length);
}

i32 tSHA1::writeAll(const u8* buffer, i32 length)
{
    if (length <= 0)
        throw eInvalidArgument("Stream read/write length must be >0");

    SHA1_Update(((SHA_CTX*)m_context), buffer, (u32)length);
    return length;
}

std::vector<u8> tSHA1::getHash() const
{
    u8 dg[SHA_DIGEST_LENGTH];
    SHA_CTX context = *((SHA_CTX*)m_context);
    SHA1_Final(dg, &context);

    std::vector<u8> v(SHA_DIGEST_LENGTH, 0);
    for (size_t i = 0; i < v.size(); i++) v[i] = dg[i];
    return v;
}

std::string tSHA1::getHashString() const
{
    std::vector<u8> hash = getHash();
    return tMD5::hashToString(hash);
}


}   // namespace crypt
}   // namespace rho
