#ifndef __rho_crypt_iHasher_h__
#define __rho_crypt_iHasher_h__


#include <rho/iWritable.h>


namespace rho
{
namespace crypt
{


class iHasher : public iWritable
{
    public:

        /**
         * Returns the hash in raw byte form.
         */
        virtual std::vector<u8> getHash() const = 0;

        /**
         * Returns the hash in string form.
         *
         * This string is a lowercase hexadecimal
         * representation of the bytes returned by
         * getHash().
         */
        virtual std::string getHashString() const = 0;

        virtual ~iHasher() { }
};


}    // namespace crypt
}    // namespace rho


#endif    // __rho_crypt_iHasher_h__
