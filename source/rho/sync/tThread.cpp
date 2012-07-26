#include "rho/sync/tThread.h"

#include "rho/sync/ebSync.h"

#include "rho/tCrashReporter.h"

#include <iostream>


namespace rho
{
namespace sync
{


static void* rho_sync_threadMain(void* thisBetterBeARunnableRef)
{
    refc<iRunnable>* runnable = (refc<iRunnable>*) thisBetterBeARunnableRef;

    try
    {
        (*runnable)->run();
    }
    catch (...)
    {
        std::cerr << "\nThread quiting due to an uncaught exception:"
                  << std::endl;
        tCrashReporter::terminate();
    }

    delete runnable;

    return NULL;
}

tThread::tThread(refc<iRunnable> runnable)
    : m_runnable(runnable),
      m_thread(0),
      m_joined(false),
      m_detached(false)
{
    int threadCreationStatus = pthread_create(
            &m_thread,
            NULL,
            rho_sync_threadMain,
            new refc<iRunnable>(runnable));

    if (threadCreationStatus != 0)
    {
        if (threadCreationStatus == EAGAIN)
        {
            throw eThreadCreationError();
        }
        else
        {
            throw eLogicError("Invalid arguments to pthread_create()?");
        }
    }
}

void tThread::join()
{
    if (m_joined)
    {
        throw eThreadCannotBeJoinedError(
                "The thread has already been joined-with.");
    }
    if (m_detached)
    {
        throw eThreadCannotBeJoinedError(
                "The thread has already been detached.");
    }

    int threadJoinStatus = pthread_join(m_thread, NULL);
    if (threadJoinStatus != 0)
    {
        throw eRuntimeError("Why isn't this thread joinable?");
    }

    m_joined = true;
}

void tThread::detach()
{
    if (m_joined)
    {
        throw eThreadCannotBeDetachedError(
                "The thread has already been joined-with.");
    }
    if (m_detached)
    {
        throw eThreadCannotBeDetachedError(
                "The thread has already been detached.");
    }

    int threadDetachStatus = pthread_detach(m_thread);
    if (threadDetachStatus != 0)
    {
        throw eRuntimeError("Why isn't this thread detachable?");
    }

    m_detached = true;
}

tThread::~tThread()
{
    if (!m_joined && !m_detached)
        detach();
}

void tThread::yield()
{
    #ifdef __linux__
    pthread_yield();
    #elif __APPLE__
    pthread_yield_np();
    #endif
}

void tThread::msleep(u64 msecs)
{
    tThread::usleep(1000 * msecs);
}

void tThread::usleep(u64 usecs)
{
    if (usecs >= 1000000)              // ::usleep must take a value <1000000
    {
        ::sleep(usecs / 1000000);
        ::usleep(usecs % 1000000);
    }
    else
    {
        ::usleep(usecs);
    }
}


}    // namespace sync
}    // namespace rho
