#include <rho/crypt/tSecureRandom.h>
#include <rho/eRho.h>
#include <rho/sync/tAutoSync.h>
#include <rho/sync/tMutex.h>
#include <rho/refc.h>

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


static sync::tMutex gSecureRandMutex;
static refc<tSecureRandom> gSecureRand;


u8   secureRand_u8()
{
    sync::tAutoSync as(gSecureRandMutex);
    if (gSecureRand == NULL)
        gSecureRand = new tSecureRandom;
    u8 val;
    if (gSecureRand->readAll(&val, 1) != 1)
        throw eRuntimeError("Cannot read from the secure random stream!");
    return val;
}

u16  secureRand_u16()
{
    sync::tAutoSync as(gSecureRandMutex);
    if (gSecureRand == NULL)
        gSecureRand = new tSecureRandom;
    u8 vals[2];
    if (gSecureRand->readAll(vals, 2) != 2)
        throw eRuntimeError("Cannot read from the secure random stream!");
    u16 val = (u16) ( (vals[0] << 8) | (vals[1]) );
    return val;
}

u32  secureRand_u32()
{
    sync::tAutoSync as(gSecureRandMutex);
    if (gSecureRand == NULL)
        gSecureRand = new tSecureRandom;
    u8 vals[4];
    if (gSecureRand->readAll(vals, 4) != 4)
        throw eRuntimeError("Cannot read from the secure random stream!");
    u32 val = (u32) ( (vals[0] << 24) | (vals[1] << 16) | (vals[2] << 8) | (vals[3]) );
    return val;
}

void secureRand_readAll(u8* buffer, i32 length)
{
    sync::tAutoSync as(gSecureRandMutex);
    if (gSecureRand == NULL)
        gSecureRand = new tSecureRandom;
    if (gSecureRand->readAll(buffer, length) != length)
        throw eRuntimeError("Cannot read from the secure random stream!");
}


class tSecureRandomInternal : public iReadable, public bNonCopyable
{
    public:

        tSecureRandomInternal()
        {
            #if __linux__ || __APPLE__ || __CYGWIN__

            m_internal = new tFileReadable("/dev/urandom");

            #elif __MINGW32__

            HCRYPTPROV hProvider = NULL;
            if (!::CryptAcquireContextW(&hProvider, 0, 0, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT | CRYPT_SILENT))
                throw eResourceAcquisitionError("Cannot create a secure random thingy on windows!");
            m_internal = hProvider;

            #else
            #error What platform are you on!?
            #endif
        }

        ~tSecureRandomInternal()
        {
            if (m_internal)
            {
                #if __linux__ || __APPLE__ || __CYGWIN__

                tFileReadable* fileReader = (tFileReadable*) m_internal;
                delete fileReader;

                #elif __MINGW32__

                ::CryptReleaseContext((HCRYPTPROV)m_internal, 0);

                #else
                #error What platform are you on!?
                #endif

                m_internal = NULL;
            }
        }

        i32 read(u8* buffer, i32 length)
        {
            #if __linux__ || __APPLE__ || __CYGWIN__

            return ((tFileReadable*)m_internal)->read(buffer, length);

            #elif __MINGW32__

            if (!::CryptGenRandom((HCRYPTPROV)m_internal, (DWORD)length, (BYTE*)buffer))
                return 0;
            else
                return length;

            #else
            #error What platform are you on!?
            #endif
        }

        i32 readAll(u8* buffer, i32 length)
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

    private:

        void* m_internal;
};


tSecureRandom::tSecureRandom()
    : m_readable(NULL)
{
    tSecureRandomInternal* internal = new tSecureRandomInternal;
    m_readable = new tBufferedReadable(internal);
}

tSecureRandom::~tSecureRandom()
{
    tSecureRandomInternal* internal
        = (tSecureRandomInternal*)m_readable->getInternalStream();
    delete m_readable;
    m_readable = NULL;
    delete internal;
    internal = NULL;
}

i32 tSecureRandom::read(u8* buffer, i32 length)
{
    return m_readable->read(buffer, length);
}

i32 tSecureRandom::readAll(u8* buffer, i32 length)
{
    return m_readable->readAll(buffer, length);
}


}    // namespace crypt
}    // namespace rho
