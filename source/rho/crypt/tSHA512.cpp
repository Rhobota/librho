#include <rho/crypt/tSHA512.h>
#include <rho/crypt/hash_utils.h>

#include "sha.h"


namespace rho
{
namespace crypt
{


tSHA512::tSHA512()
    : m_context(NULL)
{
    m_context = new SHA512_CTX;
    SHA512_Init((SHA512_CTX*)m_context);
}

tSHA512::~tSHA512()
{
    delete ((SHA512_CTX*)m_context);
    m_context = NULL;
}

i32 tSHA512::write(const u8* buffer, i32 length)
{
    return this->writeAll(buffer, length);
}

i32 tSHA512::writeAll(const u8* buffer, i32 length)
{
    if (length <= 0)
        throw eInvalidArgument("Stream read/write length must be >0");

    SHA512_Update(((SHA512_CTX*)m_context), buffer, (u32)length);
    return length;
}

std::vector<u8> tSHA512::getHash() const
{
    u8 dg[SHA512_DIGEST_LENGTH];
    SHA512_CTX context = *((SHA512_CTX*)m_context);
    SHA512_Final(dg, &context);

    std::vector<u8> v(SHA512_DIGEST_LENGTH, 0);
    for (size_t i = 0; i < v.size(); i++) v[i] = dg[i];
    return v;
}

std::string tSHA512::getHashString() const
{
    std::vector<u8> hash = getHash();
    return hashToString(hash);
}


}   // namespace crypt
}   // namespace rho
