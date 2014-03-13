#include <rho/crypt/tSHA224.h>
#include <rho/crypt/tMD5.h>

#include "sha.h"


namespace rho
{
namespace crypt
{


tSHA224::tSHA224()
    : m_context(NULL)
{
    m_context = new SHA256_CTX;
    SHA224_Init((SHA256_CTX*)m_context);
}

tSHA224::~tSHA224()
{
    delete ((SHA256_CTX*)m_context);
    m_context = NULL;
}

i32 tSHA224::write(const u8* buffer, i32 length)
{
    return this->writeAll(buffer, length);
}

i32 tSHA224::writeAll(const u8* buffer, i32 length)
{
    if (length <= 0)
        throw eInvalidArgument("Stream read/write length must be >0");

    SHA224_Update(((SHA256_CTX*)m_context), buffer, (u32)length);
    return length;
}

std::vector<u8> tSHA224::getHash() const
{
    u8 dg[SHA256_DIGEST_LENGTH];
    SHA256_CTX context = *((SHA256_CTX*)m_context);
    SHA224_Final(dg, &context);

    std::vector<u8> v(SHA224_DIGEST_LENGTH, 0);
    for (size_t i = 0; i < v.size(); i++) v[i] = dg[i];
    return v;
}

std::string tSHA224::getHashString() const
{
    std::vector<u8> hash = getHash();
    return tMD5::hashToString(hash);
}


}   // namespace crypt
}   // namespace rho
