#include <rho/sync/tThread.h>
#include <rho/sync/ebSync.h>
#include <rho/tCrashReporter.h>

#include <iostream>

#include <unistd.h>


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
      m_thread(),
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

refc<iRunnable> tThread::getRunnable() const
{
    return m_runnable;
}

tThread::~tThread()
{
    if (!m_joined && !m_detached)
        detach();
}

void tThread::yield()
{
    #if __linux__ || __CYGWIN__
    pthread_yield();
    #elif __APPLE__
    pthread_yield_np();
    #elif __MINGW32__
    // No op.
    #else
    #error What platform are you on!?
    #endif
}

void tThread::msleep(u64 msecs)
{
    tThread::usleep(1000 * msecs);
}

void tThread::usleep(u64 usecs)
{
    while (usecs >= 1000000)              // ::usleep must take a value <1000000
    {
        ::usleep(1000000-1);
        usecs -= 1000000-1;
    }
    if (usecs > 0)
    {
        ::usleep((u32)usecs);
    }
}


}    // namespace sync
}    // namespace rho
