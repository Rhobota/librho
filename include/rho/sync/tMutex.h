#ifndef __rho_sync_tMutex_h__
#define __rho_sync_tMutex_h__


#include "iSync.h"

#include "rho/bNonCopyable.h"

#include <pthread.h>   // posix headers
#include <errno.h>     //


namespace rho
{
namespace sync
{


class tMutex : public bNonCopyable, public iSync
{
    public:

        tMutex();

        /**
         * Blocks until the mutex can be acquired, then acquires it and returns.
         */
        void acquire();

        /**
         * Releases the mutex.
         */
        void release();

        ~tMutex();

    private:

        pthread_mutex_t m_mutex;
};


}     // namespace sync
}     // namespace rho


#endif    // __rho_sync_tMutex_h__
