#ifndef __rho_crypt_tSecureRandom_h__
#define __rho_crypt_tSecureRandom_h__


#include <rho/ppcheck.h>
#include <rho/iReadable.h>
#include <rho/bNonCopyable.h>

#include <string>


namespace rho
{
namespace crypt
{


// Functions used to generate secure random numbers...
// These are locked by a global mutex (think "slow" when
// many threads are involved).
u8   secureRand_u8();
u16  secureRand_u16();
u32  secureRand_u32();
u64  secureRand_u64();
void secureRand_readAll(u8* buffer, i32 length);


// If you'll need a lot of secure random numbers for a long duration,
// create your own secure random generation object.
class tSecureRandom : public iReadable, public bNonCopyable
{
    public:

        tSecureRandom();
        ~tSecureRandom();

        i32 read(u8* buffer, i32 length);
        i32 readAll(u8* buffer, i32 length);

    private:

        tBufferedReadable* m_readable;
};


// This function is used to create a Globally Unique Identifier (GUID) using the global
// secure rand object.
std::string genGUID();


}   // namespace crypt
}   // namespace rho


#endif   // __rho_crypt_tSecureRandom_h__
