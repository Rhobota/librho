#ifndef __rho_crypt_tSecureRandom_h__
#define __rho_crypt_tSecureRandom_h__


#include <rho/iReadable.h>


namespace rho
{
namespace crypt
{


class tSecureRandom : public iReadable
{
    public:

        tSecureRandom();
        ~tSecureRandom();

        i32 read(u8* buffer, i32 length);
        i32 readAll(u8* buffer, i32 length);

    private:

        void* m_internal;
};


}   // namespace crypt
}   // namespace rho


#endif   // __rho_crypt_tSecureRandom_h__