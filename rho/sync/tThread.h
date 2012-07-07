#ifndef __rho_sync_tThread_h__
#define __rho_sync_tThread_h__


#include "ebSync.h"
#include "iRunnable.h"

#include "rho/bNonCopyable.h"
#include "rho/refc.h"
#include "rho/tCrashReporter.h"

#include <pthread.h>   // posix headers
#include <errno.h>     //

#include <iostream>


namespace rho
{
namespace sync
{


class tThread : public bNonCopyable
{
    public:

        /**
         * Creates a new thread. The new thread will invoke runnable->run().
         */
        tThread(refc<iRunnable> runnable);

        /**
         * Blocks, waiting for the thread to terminate.
         * If the thread has already terminated, join()
         * will return immediately.
         */
        void join();

        /**
         * Indicates to this thread that it will never be joined-with.
         * Therefore the thread is safe to completely clean-up its state
         * automatically after it terminates.
         */
        void detach();

        /**
         * Destructs the thread object. The thread itself becomes detached
         * if it has not already been joined-with.
         */
        ~tThread();

    private:

        refc<iRunnable> m_runnable;
        pthread_t       m_thread;
        bool            m_joined;
        bool            m_detached;
};


static void* callRunWithThisNewThread(void* runnableRef)
{
    try
    {
        refc<iRunnable> runnable = *((refc<iRunnable>*) runnableRef);
        runnable->run();
    }
    catch (...)
    {
        std::cerr << "\nThread quiting due to an uncaught exception:"
                  << std::endl;
        rho::terminate();
    }

    return NULL;
}

tThread::tThread(refc<iRunnable> runnable)
    : m_runnable(runnable),
      m_thread(0),
      m_joined(false),
      m_detached(false)
{
    int threadCreationStatus =
        pthread_create(&m_thread, NULL, callRunWithThisNewThread, &m_runnable);

    if (threadCreationStatus != 0)
    {
        if (threadCreationStatus == EAGAIN)
        {
            throw eThreadCreationError();
        }
        else
        {
            throw std::logic_error("Invalid arguments to pthread_create()?");
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
        throw std::runtime_error("Why isn't this thread joinable?");
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
        throw std::runtime_error("Why isn't this thread detachable?");
    }

    m_detached = true;
}

tThread::~tThread()
{
    if (!m_joined && !m_detached)
        detach();
}


}    // namespace sync
}    // namespace rho


#endif   // __rho_sync_tThread_h__
