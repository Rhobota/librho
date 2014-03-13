#include <rho/crypt/tSHA256.h>
#include <rho/crypt/tMD5.h>

#include "sha.h"


namespace rho
{
namespace crypt
{


tSHA256::tSHA256()
    : m_context(NULL)
{
    m_context = new SHA256_CTX;
    SHA256_Init((SHA256_CTX*)m_context);
}

tSHA256::~tSHA256()
{
    delete ((SHA256_CTX*)m_context);
    m_context = NULL;
}

i32 tSHA256::write(const u8* buffer, i32 length)
{
    return this->writeAll(buffer, length);
}

i32 tSHA256::writeAll(const u8* buffer, i32 length)
{
    if (length <= 0)
        throw eInvalidArgument("Stream read/write length must be >0");

    SHA256_Update(((SHA256_CTX*)m_context), buffer, (u32)length);
    return length;
}

std::vector<u8> tSHA256::getHash() const
{
    u8 dg[SHA256_DIGEST_LENGTH];
    SHA256_CTX context = *((SHA256_CTX*)m_context);
    SHA256_Final(dg, &context);

    std::vector<u8> v(SHA256_DIGEST_LENGTH, 0);
    for (size_t i = 0; i < v.size(); i++) v[i] = dg[i];
    return v;
}

std::string tSHA256::getHashString() const
{
    std::vector<u8> hash = getHash();
    return tMD5::hashToString(hash);
}


}   // namespace crypt
}   // namespace rho
