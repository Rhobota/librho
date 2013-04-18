#ifndef __rho_crypt_tRSA_h__
#define __rho_crypt_tRSA_h__


#include <rho/algo/tBigInteger.h>

#include <string>
#include <vector>


namespace rho
{
namespace crypt
{


class tRSA
{
    public:

        tRSA(std::string keyfilepath);

        bool hasPrivateKey()    const;
        u32  maxMessageLength() const;

        // The following can be done by any tRSA object:
        std::vector<u8> encrypt(std::vector<u8> pt)                             const;
        bool            verify(std::vector<u8> hash, std::vector<u8> signature) const;

        // The following can only be done for tRSA objects which have the private key:
        std::vector<u8> decrypt(std::vector<u8> ct) const;
        std::vector<u8> sign(std::vector<u8> hash)  const;

    private:

        algo::tBigInteger n;    // modulus
        algo::tBigInteger e;    // public key
        algo::tBigInteger d;    // private key
};


}   // namespace crypt
}   // namespace rho


#endif   // __rho_crypt_tRSA_h__
