#include "rho/sync/tMutex.h"

#include "rho/sync/ebSync.h"


namespace rho
{
namespace sync
{


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
            throw eLogicError("Incorrect m_mutex value?");
    }
}

void tMutex::release()
{
    int mutexUnlockStatus = pthread_mutex_unlock(&m_mutex);
    if (mutexUnlockStatus != 0)
    {
        if (mutexUnlockStatus == EPERM)
            throw eLogicError("The current thread is trying to unlock a mutex which it does NOT own.");
        else
            throw eLogicError("Incorrect m_mutex value?");
    }
}

tMutex::~tMutex()
{
    pthread_mutex_destroy(&m_mutex);
}


}     // namespace sync
}     // namespace rho
