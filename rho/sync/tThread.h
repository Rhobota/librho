#ifndef __rho_sync_tThread_h__
#define __rho_sync_tThread_h__


#include "ebSync.h"
#include "iRunnable.h"

#include "rho/bNonCopyable.h"
#include "rho/refc.h"
#include "rho/tCrashReporter.h"
#include "rho/types.h"

#include <pthread.h>   //
#include <errno.h>     // posix headers
#include <unistd.h>    //

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

    public:

        /**
         * Causes the calling thread to relinquish the CPU.
         */
        static void yield();

        /**
         * Causes the currently running thread to sleep for the given
         * number of milliseconds (1,000 milliseconds in a second)
         */
        static void msleep(u64 msecs);

        /**
         * Causes the currently running thread to sleep for the given
         * number of microseconds. (1,000,000 microseconds in a second)
         */
        static void usleep(u64 usecs);

    private:

        refc<iRunnable> m_runnable;
        pthread_t       m_thread;
        bool            m_joined;
        bool            m_detached;
};


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
        rho::terminate();
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


#endif   // __rho_sync_tThread_h__
