#include <rho/crypt/tRSA.h>
#include <rho/eRho.h>
#include <rho/crypt/tSecureRandom.h>

#include <cstdlib>
using namespace std;


namespace rho
{
namespace crypt
{


tRSA::tRSA(iReadable* readable)
    : n(0), e(0), d(0)
{
    unpack(readable);
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
    u8 rnd = 0;
    while (rnd == 0)
        rnd = secureRand_u8();

    vector<u8> padded = bytes;
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

bool tRSA::verSig(vector<u8> hash, vector<u8> signature) const
{
    algo::tBigInteger sigAsInt(signature);
    if (sigAsInt >= n)
        throw eInvalidArgument("The signature value must be less than the modulus.");
    algo::tBigInteger hashAsInt = sigAsInt.modPow(e, n);
    return unpad(hashAsInt.getBytes()) == hash;
}

algo::tBigInteger tRSA::getModulus() const
{
    return n;
}

algo::tBigInteger tRSA::getPubKey()  const
{
    return e;
}

algo::tBigInteger tRSA::getPrivKey() const
{
    return d;
}

void tRSA::pack(iWritable* out) const
{
    rho::pack(out, n.getBytes());
    rho::pack(out, e.getBytes());
    rho::pack(out, d.getBytes());
}

void tRSA::unpack(iReadable* in)
{
    std::vector<u8> bytes;
    rho::unpack(in, bytes); n = algo::tBigInteger(bytes);
    rho::unpack(in, bytes); e = algo::tBigInteger(bytes);
    rho::unpack(in, bytes); d = algo::tBigInteger(bytes);
}

tRSA tRSA::generate(u32 numBits, u32 numRounds)
{
    tByteWritable out;
    generate(numBits, numRounds, &out);
    tByteReadable in(out.getBuf());
    return tRSA(&in);
}

void tRSA::generate(u32 numBits, u32 numRounds, iWritable* out)
{
    // Generate the secret primes: p and q
    algo::tBigInteger p(0);
    algo::tBigInteger q(0);
    do
    {
        p = algo::tBigInteger::genPseudoPrime(numBits/2, numRounds);
        q = algo::tBigInteger::genPseudoPrime(numBits/2, numRounds);
    } while (p == q);

    // Compute the known modulus: n
    algo::tBigInteger n = p * q;

    // Compute the secret Euler's totient: m
    algo::tBigInteger m = (p-1) * (q-1);

    // Find an appropriate public key: e
    algo::tBigInteger e(65537);
    while (algo::GCD(e, m) > 1)
        e += 2;

    // Compute the private key: d
    algo::tBigInteger d(0), aux(0);
    algo::extendedGCD(e, m, d, aux);
    while (d.isNegative())
        d += m;

    // Pack the required values: n, e, and d
    rho::pack(out, n.getBytes());
    rho::pack(out, e.getBytes());
    rho::pack(out, d.getBytes());

    // The following are not current used by this class, but
    // they might be used in the future for the Chinese Remainder
    // Algorithm, or for something else...
    rho::pack(out, p.getBytes());
    rho::pack(out, q.getBytes());
}


}   // namespace crypt
}   // namespace rho
