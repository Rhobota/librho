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
    FILE* fp = fopen("/dev/random", "r");
    if (!fp)
        throw eResourceAcquisitionError("Cannot open /dev/random");
    m_internal = fp;
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
        fclose((FILE*)m_internal);
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
    return readAll(buffer, length);
}

i32 tSecureRandom::readAll(u8* buffer, i32 length)
{
    #if __linux__ || __APPLE__ || __CYGWIN__
    for (i32 i = 0; i < length; i++)
        buffer[i] = (u8) fgetc((FILE*)m_internal);
    return length;
    #elif __MINGW32__
    if (!::CryptGenRandom((HCRYPTPROV)m_internal, (DWORD)length, (BYTE*)buffer))
        throw eResourceAcquisitionError("Cannot generate any secure random bytes on windows!");
    #else
    #error What platform are you on!?
    #endif
}


}    // namespace crypt
}    // namespace rho
