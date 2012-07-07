#ifndef __rho_sync_tMutex_h__
#define __rho_sync_tMutex_h__


#include "ebSync.h"
#include "iSync.h"

#include "rho/bNonCopyable.h"

#include <pthread.h>   // posix headers
#include <errno.h>


namespace rho
{
namespace sync
{


class tMutex : public iSync, public bNonCopyable
{
    public:

        tMutex();

        void acquire();
        void release();

        ~tMutex();

    private:

        pthread_mutex_t m_mutex;
};


tMutex::tMutex()
    : m_mutex()
{
    int mutexCreationStatus = pthread_mutex_init(&m_mutex, NULL);
    if (mutexCreationStatus != 0)
    {
        throw eMutexCreationError();
    }
}

void tMutex::acquire()
{
    int mutexLockStatus = pthread_mutex_lock(&m_mutex);
    if (mutexLockStatus != 0)
    {
        if (mutexLockStatus == EDEADLK)
            throw eDeadlockDetected();
        else
            throw std::logic_error("Incorrect m_mutex value?");
    }
}

void tMutex::release()
{
    int mutexUnlockStatus = pthread_mutex_unlock(&m_mutex);
    if (mutexUnlockStatus != 0)
    {
        if (mutexUnlockStatus == EPERM)
            throw std::runtime_error("The current thread is trying to unlock a mutex which it does NOT own.");
        else
            throw std::logic_error("Incorrect m_mutex value?");
    }
}

tMutex::~tMutex()
{
    pthread_mutex_destroy(&m_mutex);
}


}     // namespace sync
}     // namespace rho


#endif    // __rho_sync_tMutex_h__
