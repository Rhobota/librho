#include <rho/crypt/tWhirlpool.h>
#include <rho/crypt/hash_utils.h>

#include "Whirlpool.ipp"


namespace rho
{
namespace crypt
{


tWhirlpool::tWhirlpool()
    : m_context(NULL)
{
    m_context = malloc(sizeof(struct NESSIEstruct));
    NESSIEinit((struct NESSIEstruct*)m_context);
}

tWhirlpool::~tWhirlpool()
{
    free(m_context);
    m_context = NULL;
}

i32 tWhirlpool::write(const u8* buffer, i32 length)
{
    return this->writeAll(buffer, length);
}

i32 tWhirlpool::writeAll(const u8* buffer, i32 length)
{
    if (length <= 0)
        throw eInvalidArgument("Stream read/write length must be >0");

    NESSIEadd(buffer, (u32)(length*8), ((struct NESSIEstruct*)m_context));
    return length;
}

std::vector<u8> tWhirlpool::getHash() const
{
    u8 dg[DIGESTBYTES];
    struct NESSIEstruct context = *((struct NESSIEstruct*)m_context);
    NESSIEfinalize(&context, dg);

    std::vector<u8> v(DIGESTBYTES, 0);
    for (size_t i = 0; i < v.size(); i++) v[i] = dg[i];
    return v;
}

std::string tWhirlpool::getHashString() const
{
    std::vector<u8> hash = getHash();
    return hashToString(hash);
}


}   // namespace crypt
}   // namespace rho
