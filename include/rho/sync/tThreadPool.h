#ifndef __rho_sync_tThreadPool_h__
#define __rho_sync_tThreadPool_h__


#include <rho/sync/tThread.h>
#include <rho/sync/tPCQ.h>

#include <set>
#include <utility>
#include <vector>


namespace rho
{
namespace sync
{


class tThreadPool : public bNonCopyable
{
    public:

        tThreadPool(u32 numThreads);
        ~tThreadPool();

        u32 getNumThreads() const;

        typedef u64 tTaskKey;

        tTaskKey push(refc<iRunnable> runnable);
        void     wait(tTaskKey key);

    private:

        tTaskKey m_incrementKey();
        void     m_addToCompleted(tTaskKey key);
        tTaskKey m_push(refc<iRunnable> runnable);

    private:

        typedef std::pair< tTaskKey,refc<iRunnable> > tTask;

        std::vector< refc<tThread> > m_threads;

        tPCQ<tTask>                  m_queue;
        tTaskKey                     m_nextKey;

        std::set<tTaskKey>           m_completed;
        pthread_mutex_t              m_completedMutex;
        pthread_cond_t               m_completedWasUpdated;

        friend class tWorker;
};


}    // namespace sync
}    // namespace rho


#endif   // __rho_sync_tThreadPool_h__
