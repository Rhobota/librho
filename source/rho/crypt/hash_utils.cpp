#include <rho/crypt/hash_utils.h>
#include <rho/crypt/tMD5.h>
#include <rho/crypt/tSHA1.h>
#include <rho/crypt/tSHA224.h>
#include <rho/crypt/tSHA256.h>
#include <rho/crypt/tSHA384.h>
#include <rho/crypt/tSHA512.h>

#include <cassert>
#include <sstream>
#include <iomanip>

using std::string;
using std::vector;


namespace rho
{
namespace crypt
{


string hashToString(const vector<u8>& hash)
{
    std::ostringstream out;
    for (u32 i = 0; i < hash.size(); i++)
        out << std::hex << std::setfill('0') << std::setw(2) << ((u32)hash[i]);
    return out.str();
}


template <class Hasher, size_t blocksize>
vector<u8> hmac(vector<u8> key, const vector<u8>& message)
{
    assert(blocksize > 0);

    // If the key is too long, shorten it.
    if (key.size() > blocksize)
    {
        Hasher h;
        h.write(&key[0], (i32)key.size());
        key = h.getHash();
        assert(key.size() <= blocksize);
    }

    // If the key is (now) too short, pad it on the right with zeros.
    while (key.size() < blocksize)
        key.push_back(0x00);

    // Create the outer padding vector.
    vector<u8> o_key_pad(blocksize, 0x5c);
    for (size_t i = 0; i < blocksize; i++)
        o_key_pad[i] ^= key[i];

    // Create the inner padding vector.
    vector<u8> i_key_pad(blocksize, 0x36);
    for (size_t i = 0; i < blocksize; i++)
        i_key_pad[i] ^= key[i];

    // Compute the inner hash part.
    Hasher h1;
    h1.write(&i_key_pad[0], (i32)i_key_pad.size());
    if (message.size() > 0)
        h1.write(&message[0], (i32)message.size());
    vector<u8> res1 = h1.getHash();
    assert(res1.size() > 0);

    // Compute the outer hash part.
    Hasher h2;
    h2.write(&o_key_pad[0], (i32)o_key_pad.size());
    h2.write(&res1[0], (i32)res1.size());
    return h2.getHash();
}


vector<u8> hmac_md5(const vector<u8>& key, const vector<u8>& message)
{
    return hmac<crypt::tMD5, 64>(key, message);
}


vector<u8> hmac_sha1(const vector<u8>& key, const vector<u8>& message)
{
    return hmac<crypt::tSHA1, 64>(key, message);
}


vector<u8> hmac_sha224(const vector<u8>& key, const vector<u8>& message)
{
    return hmac<crypt::tSHA224, 64>(key, message);
}


vector<u8> hmac_sha256(const vector<u8>& key, const vector<u8>& message)
{
    return hmac<crypt::tSHA256, 64>(key, message);
}


vector<u8> hmac_sha384(const vector<u8>& key, const vector<u8>& message)
{
    return hmac<crypt::tSHA384, 128>(key, message);
}


vector<u8> hmac_sha512(const vector<u8>& key, const vector<u8>& message)
{
    return hmac<crypt::tSHA512, 128>(key, message);
}


}   // namespace crypt
}   // namespace rho
