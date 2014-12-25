#ifndef __rho_crypt_tWhirlpool_h__
#define __rho_crypt_tWhirlpool_h__


#include <rho/ppcheck.h>
#include <rho/crypt/iHasher.h>


namespace rho
{
namespace crypt
{


class tWhirlpool : public iHasher, public bNonCopyable
{
    public:

        tWhirlpool();
        ~tWhirlpool();

        i32 write(const u8* buffer, i32 length);
        i32 writeAll(const u8* buffer, i32 length);

        /**
         * Returns the hash in vector form.
         * The returned vector always has length of 64.
         */
        std::vector<u8> getHash() const;

        /**
         * Returns the hash in string form.
         * The returned string always is hexadecimal
         * and always has length 128.
         */
        std::string getHashString() const;

    private:

        void* m_context;
};


}   // namespace crypt
}   // namespace rho


#endif   // __rho_crypt_tWhirlpool_h__
