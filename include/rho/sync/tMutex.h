#ifndef __rho_sync_tMutex_h__
#define __rho_sync_tMutex_h__


#include <rho/ppcheck.h>
#include <rho/bNonCopyable.h>
#include <rho/sync/iSync.h>
#include <rho/sysinclude.h>


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
        void acquire() const;

        /**
         * Releases the mutex.
         */
        void release() const;

        ~tMutex();

    private:

        pthread_mutex_t m_mutex;

        friend class tCondition;
};


}     // namespace sync
}     // namespace rho


#endif    // __rho_sync_tMutex_h__
