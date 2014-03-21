#ifndef __rho_crypt_tSecureStream_h__
#define __rho_crypt_tSecureStream_h__


#include <rho/refc.h>

#include <rho/iReadable.h>
#include <rho/iWritable.h>
#include <rho/iFlushable.h>
#include <rho/bNonCopyable.h>

#include <rho/crypt/tReadableAES.h>
#include <rho/crypt/tWritableAES.h>
#include <rho/crypt/tRSA.h>


namespace rho
{
namespace crypt
{


class tSecureStream : public iReadable, public iWritable,
                      public iFlushable, public bNonCopyable
{
    public:

        tSecureStream(iReadable* internalReadable,
                      iWritable* internalWritable,
                      const tRSA& rsa,
                      std::string appGreeting);

        ~tSecureStream();

        i32 read(u8* buffer, i32 length);
        i32 readAll(u8* buffer, i32 length);

        i32 write(const u8* buffer, i32 length);
        i32 writeAll(const u8* buffer, i32 length);

        bool flush();

    private:

        void m_setupServer(const tRSA& rsa, std::string appGreeting);
        void m_setupClient(const tRSA& rsa, std::string appGreeting);

    private:

        iReadable* m_internal_readable;
        iWritable* m_internal_writable;

        refc<tReadableAES> m_readable;
        refc<tWritableAES> m_writable;
};


}   // namespace crypt
}   // namespace rho


#endif   // __rho_crypt_tSecureStream_h__
