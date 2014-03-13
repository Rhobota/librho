#ifndef __rho_crypt_tMD5_h__
#define __rho_crypt_tMD5_h__


#include <rho/crypt/iHasher.h>


namespace rho
{
namespace crypt
{


class tMD5 : public iHasher, public bNonCopyable
{
    public:

        tMD5();
        ~tMD5();

        i32 write(const u8* buffer, i32 length);
        i32 writeAll(const u8* buffer, i32 length);

        /**
         * Returns the hash in vector form.
         * The returned vector always has length of 16.
         */
        std::vector<u8> getHash() const;

        /**
         * Returns the hash in string form.
         * The returned string always is hexadecimal
         * and always has length 32.
         */
        std::string getHashString() const;

    public:

        /**
         * Converts a hash (aka, a vector of bytes) to a string
         * representation (aka, a lowercase concatenation of bytes,
         * each byte represented by two hex characters).
         */
        static std::string hashToString(const std::vector<u8>& hash);

    private:

        void* m_context;
};


}   // namespace crypt
}   // namespace rho


#endif   // __rho_crypt_tMD5_h__
