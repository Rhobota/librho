#ifndef __rho_crypt_tRSA_h__
#define __rho_crypt_tRSA_h__


#include <rho/ppcheck.h>
#include <rho/iPackable.h>
#include <rho/bNonCopyable.h>
#include <rho/algo/tBigInteger.h>

#include <string>
#include <vector>


namespace rho
{
namespace crypt
{


class tRSA : public iPackable
{
    public:

        tRSA(iReadable* readable);

        tRSA(std::vector<u8> modulus, std::vector<u8> publicKey);
        tRSA(std::vector<u8> modulus, std::vector<u8> publicKey, std::vector<u8> privateKey);

        tRSA(std::string modulus, std::string publicKey);
        tRSA(std::string modulus, std::string publicKey, std::string privateKey);

        // This tRSA's capabilities:
        bool hasPrivateKey()    const;
        u32  maxMessageLength() const;

        // The following can be done by any tRSA object:
        std::vector<u8> encrypt(std::vector<u8> pt)                             const;
        bool            verSig(std::vector<u8> hash, std::vector<u8> signature) const;  // <-- short for "verify signature"

        // The following can only be done for tRSA objects which have the private key:
        std::vector<u8> decrypt(std::vector<u8> ct) const;
        std::vector<u8> sign(std::vector<u8> hash)  const;

        // Access to the underlying integers:
        algo::tBigInteger getModulus() const;
        algo::tBigInteger getPubKey()  const;
        algo::tBigInteger getPrivKey() const;

    public:

        static tRSA generate(u32 numBits, u32 numRounds);

        static void generate(u32 numBits, u32 numRounds, iWritable* out);

    public:

        // iPackable interface:
        void pack(iWritable* out) const;
        void unpack(iReadable* in);

    private:

        algo::tBigInteger n;    // modulus
        algo::tBigInteger e;    // public key
        algo::tBigInteger d;    // private key
};


}   // namespace crypt
}   // namespace rho


#endif   // __rho_crypt_tRSA_h__
