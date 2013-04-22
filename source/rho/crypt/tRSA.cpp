#include <rho/crypt/tRSA.h>
#include <rho/eRho.h>

#include <cstdlib>
#include <fstream>
using namespace std;


namespace rho
{
namespace crypt
{


tRSA::tRSA(string keyfilepath)
    : n(0), e(0), d(0)
{
    ifstream in(keyfilepath.c_str());
    if (!in)
        throw eResourceAcquisitionError("Cannot open RSA key file!");
    in >> n >> e >> d;
    in.close();
    if (n.isZero() || e.isZero())
        throw eInvalidArgument("Neither the modulus nor the public key may be zero.");
}

tRSA::tRSA(vector<u8> modulus, vector<u8> publicKey)
    : n(modulus), e(publicKey), d(0)
{
}

tRSA::tRSA(vector<u8> modulus, vector<u8> publicKey, vector<u8> privateKey)
    : n(modulus), e(publicKey), d(privateKey)
{
}

tRSA::tRSA(string modulus, string publicKey)
    : n(modulus), e(publicKey), d(0)
{
}

tRSA::tRSA(string modulus, string publicKey, string privateKey)
    : n(modulus), e(publicKey), d(privateKey)
{
}

bool tRSA::hasPrivateKey() const
{
    return !d.isZero();
}

u32  tRSA::maxMessageLength() const
{
    return (u32) (n.getBytes().size() - 2);
}

static
vector<u8> pad(const vector<u8>& bytes)
{
    vector<u8> padded = bytes;
    u8 rnd = (u8) ((rand() % 255) + 1);
    padded.push_back(rnd);
    return padded;
}

static
vector<u8> unpad(const vector<u8>& bytes)
{
    vector<u8> unpadded = bytes;
    unpadded.pop_back();
    return unpadded;
}

vector<u8> tRSA::encrypt(vector<u8> pt) const
{
    algo::tBigInteger ptAsInt(pad(pt));
    if (ptAsInt >= n)
        throw eInvalidArgument("The plain text value must be less than the modulus.");
    algo::tBigInteger ctAsInt = ptAsInt.modPow(e, n);
    return ctAsInt.getBytes();
}

vector<u8> tRSA::decrypt(vector<u8> ct) const
{
    if (!hasPrivateKey())
        throw eLogicError("Cannot decrypt cypher text unless the private key is available.");
    algo::tBigInteger ctAsInt(ct);
    if (ctAsInt >= n)
        throw eInvalidArgument("The cypher text value must be less than the modulus.");
    algo::tBigInteger ptAsInt = ctAsInt.modPow(d, n);
    return unpad(ptAsInt.getBytes());
}

vector<u8> tRSA::sign(vector<u8> hash) const
{
    if (!hasPrivateKey())
        throw eLogicError("Cannot sign a hash unless the private key is available.");
    algo::tBigInteger hashAsInt(pad(hash));
    if (hashAsInt >= n)
        throw eInvalidArgument("The hash value must be less than the modulus.");
    algo::tBigInteger sigAsInt = hashAsInt.modPow(d, n);
    return sigAsInt.getBytes();
}

bool tRSA::verify(vector<u8> hash, vector<u8> signature) const
{
    algo::tBigInteger sigAsInt(signature);
    if (sigAsInt >= n)
        throw eInvalidArgument("The signature value must be less than the modulus.");
    algo::tBigInteger hashAsInt = sigAsInt.modPow(e, n);
    return unpad(hashAsInt.getBytes()) == hash;
}


}   // namespace crypt
}   // namespace rho
