#include <rho/crypt/tSHA384.h>
#include <rho/crypt/tMD5.h>

#include "sha.h"


namespace rho
{
namespace crypt
{


tSHA384::tSHA384()
    : m_context(NULL)
{
    m_context = new SHA512_CTX;
    SHA384_Init((SHA512_CTX*)m_context);
}

tSHA384::~tSHA384()
{
    delete ((SHA512_CTX*)m_context);
    m_context = NULL;
}

i32 tSHA384::write(const u8* buffer, i32 length)
{
    return this->writeAll(buffer, length);
}

i32 tSHA384::writeAll(const u8* buffer, i32 length)
{
    if (length <= 0)
        throw eInvalidArgument("Stream read/write length must be >0");

    SHA384_Update(((SHA512_CTX*)m_context), buffer, (u32)length);
    return length;
}

std::vector<u8> tSHA384::getHash() const
{
    u8 dg[SHA512_DIGEST_LENGTH];
    SHA512_CTX context = *((SHA512_CTX*)m_context);
    SHA384_Final(dg, &context);

    std::vector<u8> v(SHA384_DIGEST_LENGTH, 0);
    for (size_t i = 0; i < v.size(); i++) v[i] = dg[i];
    return v;
}

std::string tSHA384::getHashString() const
{
    std::vector<u8> hash = getHash();
    return tMD5::hashToString(hash);
}


}   // namespace crypt
}   // namespace rho
