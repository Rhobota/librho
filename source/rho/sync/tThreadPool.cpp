#include <rho/sync/tThreadPool.h>


namespace rho
{
namespace sync
{


class tAutoLock
{
    public:

        tAutoLock(pthread_mutex_t* mutex)
            : m_mutex(mutex)
        {
            if (pthread_mutex_lock(m_mutex) != 0)
                throw eRuntimeError("Cannot lock thread pool mutex!");
        }

        ~tAutoLock()
        {
            if (pthread_mutex_unlock(m_mutex) != 0)
                throw eRuntimeError("Cannot unlock thread pool mutex!");
            m_mutex = NULL;
        }

    private:

        pthread_mutex_t* m_mutex;
};


class tWorker : public iRunnable
{
    public:

        tWorker(tThreadPool* pool)
            : m_pool(pool)
        {
        }

        void run()
        {
            while (true)
            {
                tThreadPool::tTask task = m_pool->m_queue.pop();
                if (task.second == NULL)
                    break;
                try
                {
                    task.second->run();
                }
                catch (std::exception& e)
                {
                    std::cerr << "Thread pool task " << task.first
                              << " threw an exception: "
                              << e.what() << std::endl;
                }
                catch (...)
                {
                    std::cerr << "Thread pool task " << task.first
                              << " threw an unknown exception type."
                              << std::endl;
                }
                m_pool->m_addToCompleted(task.first);
            }
        }

    private:

        tThreadPool* m_pool;
};


tThreadPool::tThreadPool(u32 numThreads)
    : m_queue(8, kGrow), m_nextKey(1)
{
    if (numThreads == 0)
    {
        throw eInvalidArgument("There must be at least one thread in the pool.");
    }
    if (pthread_mutex_init(&m_completedMutex, NULL) != 0)
    {
        throw eMutexCreationError();
    }
    if (pthread_cond_init(&m_completedWasUpdated, NULL) != 0)
    {
        pthread_mutex_destroy(&m_completedMutex);
        throw eConditionCreationError();
    }
    for (u32 i = 0; i < numThreads; i++)
    {
        refc<iRunnable> worker(new tWorker(this));
        refc<tThread> thread(new tThread(worker));
        m_threads.push_back(thread);
    }
}

tThreadPool::~tThreadPool()
{
    for (u32 i = 0; i < m_threads.size(); i++)
    {
        refc<iRunnable> runnable(NULL);
        m_push(runnable);
    }
    for (u32 i = 0; i < m_threads.size(); i++)
    {
        m_threads[i]->join();
    }
    m_threads.clear();

    pthread_mutex_destroy(&m_completedMutex);
    pthread_cond_destroy(&m_completedWasUpdated);
}

u32 tThreadPool::getNumThreads() const
{
    return (u32)m_threads.size();
}

tThreadPool::tTaskKey tThreadPool::push(refc<iRunnable> runnable)
{
    if (runnable == NULL)
        throw eInvalidArgument("The runnable may not be NULL.");
    return m_push(runnable);
}

void tThreadPool::wait(tTaskKey key)
{
    tAutoLock autolock(&m_completedMutex);
    while (m_completed.find(key) == m_completed.end())
    {
        if (m_forgotten.find(key) != m_forgotten.end())
            throw eRuntimeError("Don't wait on a key you've forgotten!");
        if (pthread_cond_wait(&m_completedWasUpdated, &m_completedMutex) != 0)
            throw eRuntimeError("Why can't I wait on the condition?");
    }
    m_completed.erase(key);
}

void tThreadPool::forget(tTaskKey key)
{
    tAutoLock autolock(&m_completedMutex);
    if (m_completed.find(key) != m_completed.end())
        m_completed.erase(key);
    else
        m_forgotten.insert(key);
}

tThreadPool::tTaskKey tThreadPool::m_incrementKey()
{
    tAutoLock autolock(&m_completedMutex);
    tTaskKey key = m_nextKey++;
    return key;
}

void tThreadPool::m_addToCompleted(tTaskKey key)
{
    tAutoLock autolock(&m_completedMutex);
    if (m_forgotten.find(key) != m_forgotten.end())
        m_forgotten.erase(key);
    else
    {
        m_completed.insert(key);
        if (pthread_cond_broadcast(&m_completedWasUpdated) != 0)
            throw eRuntimeError("Why can't I broadcast?");
    }
}

tThreadPool::tTaskKey tThreadPool::m_push(refc<iRunnable> runnable)
{
    tTaskKey key = m_incrementKey();
    tTask task = std::make_pair(key, runnable);
    m_queue.push(task);
    return key;
}


}   // namespace sync
}   // namespace rho
