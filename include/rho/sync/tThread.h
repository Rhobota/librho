#ifndef __rho_sync_tThread_h__
#define __rho_sync_tThread_h__


#include <rho/bNonCopyable.h>
#include <rho/refc.h>
#include <rho/types.h>

#include <rho/sync/iRunnable.h>

#include <pthread.h>   //
#include <errno.h>     // posix headers
#include <unistd.h>    //


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
         * Returns the runnable that this thread is running.
         */
        refc<iRunnable> getRunnable() const;

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


}    // namespace sync
}    // namespace rho


#endif   // __rho_sync_tThread_h__
