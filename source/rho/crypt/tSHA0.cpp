#include <rho/crypt/tSHA0.h>
#include <rho/crypt/hash_utils.h>

#include "sha.h"


namespace rho
{
namespace crypt
{


tSHA0::tSHA0()
    : m_context(NULL)
{
    m_context = new SHA_CTX;
    SHA_Init((SHA_CTX*)m_context);
}

tSHA0::~tSHA0()
{
    delete ((SHA_CTX*)m_context);
    m_context = NULL;
}

i32 tSHA0::write(const u8* buffer, i32 length)
{
    return this->writeAll(buffer, length);
}

i32 tSHA0::writeAll(const u8* buffer, i32 length)
{
    if (length <= 0)
        throw eInvalidArgument("Stream read/write length must be >0");

    SHA_Update(((SHA_CTX*)m_context), buffer, (u32)length);
    return length;
}

std::vector<u8> tSHA0::getHash() const
{
    u8 dg[SHA_DIGEST_LENGTH];
    SHA_CTX context = *((SHA_CTX*)m_context);
    SHA_Final(dg, &context);

    std::vector<u8> v(SHA_DIGEST_LENGTH, 0);
    for (size_t i = 0; i < v.size(); i++) v[i] = dg[i];
    return v;
}

std::string tSHA0::getHashString() const
{
    std::vector<u8> hash = getHash();
    return hashToString(hash);
}


}   // namespace crypt
}   // namespace rho
