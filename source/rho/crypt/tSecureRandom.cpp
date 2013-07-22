#include <rho/crypt/tSecureRandom.h>
#include <rho/eRho.h>

#if __MINGW32__
#include <windows.h>
#include <Wincrypt.h>
#endif

#include <cstdlib>
#include <cstdio>


namespace rho
{
namespace crypt
{


tSecureRandom::tSecureRandom()
{
    #if __linux__ || __APPLE__ || __CYGWIN__

    m_internal = new tBufferedReadable(new tFileReadable("/dev/urandom"));

    #elif __MINGW32__

    HCRYPTPROV hProvider = NULL;
    if (!::CryptAcquireContextW(&hProvider, 0, 0, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT | CRYPT_SILENT))
        throw eResourceAcquisitionError("Cannot create a secure random thingy on windows!");
    m_internal = hProvider;

    #else
    #error What platform are you on!?
    #endif
}

tSecureRandom::~tSecureRandom()
{
    if (m_internal)
    {
        #if __linux__ || __APPLE__ || __CYGWIN__

        tBufferedReadable* bufReader = (tBufferedReadable*)m_internal;
        iReadable* fileReader = bufReader->getInternalStream();
        delete bufReader;
        delete fileReader;

        #elif __MINGW32__

        ::CryptReleaseContext((HCRYPTPROV)m_internal, 0);

        #else
        #error What platform are you on!?
        #endif

        m_internal = NULL;
    }
}

i32 tSecureRandom::read(u8* buffer, i32 length)
{
    #if __linux__ || __APPLE__ || __CYGWIN__

    return ((tBufferedReadable*)m_internal)->read(buffer, length);

    #elif __MINGW32__

    if (!::CryptGenRandom((HCRYPTPROV)m_internal, (DWORD)length, (BYTE*)buffer))
        throw eResourceAcquisitionError("Cannot generate any secure random bytes on windows!");

    #else
    #error What platform are you on!?
    #endif
}

i32 tSecureRandom::readAll(u8* buffer, i32 length)
{
    i32 amountRead = 0;
    while (amountRead < length)
    {
        i32 n = read(buffer+amountRead, length-amountRead);
        if (n <= 0)
            return (amountRead>0) ? amountRead : n;
        amountRead += n;
    }
    return amountRead;
}


}    // namespace crypt
}    // namespace rho
