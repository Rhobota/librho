#include <rho/sync/tThreadPool.h>
#include <rho/bNonCopyable.h>
#include <rho/tCrashReporter.h>
#include <rho/tTest.h>


using namespace rho;


// Long-running test
const int kNumThreadsInPool = 32;
const int kLargeNumTasks = 1000;
const int kBatchSize = 200;
const int kLargeWorkerCountTo = 10000000;

// Short-running test
const int kMaxThreadsInPool = 32;
const int kShortNumTasks = 100000;
const int kMaxBatchSize = 200;
const int kSmallWorkerCountTo = 20000;
const int kNumRepeatTests = 10;


class tMyTask : public sync::iRunnable, public bNonCopyable
{
    public:

        tMyTask(int amountOfWork)
            : m_amountOfWork(amountOfWork),
              m_answer(0),
              m_threadDidRunYay(false)
        {
        }

        void run()
        {
            int answer = 0;
            for (int i = 0; i < m_amountOfWork; i++)
                answer *= (i+4)-i*i;
            m_answer = answer;
            m_threadDidRunYay = true;
        }

        int getAnswer()
        {
            return m_answer;
        }

        bool didRun()
        {
            return m_threadDidRunYay;
        }

    private:

        int m_amountOfWork;
        int m_answer;
        bool m_threadDidRunYay;
};


void longPoolTest(const tTest& t)
{
    sync::tThreadPool pool(kNumThreadsInPool);

    t.assert(pool.getNumThreads() == (u32)kNumThreadsInPool);

    for (int i = 0; i < kLargeNumTasks; i += kBatchSize)
    {
        std::vector< refc<tMyTask> > tasks(kBatchSize);

        std::vector< sync::tThreadPool::tTaskKey > keys(kBatchSize);

        for (int j = 0; j < kBatchSize; j++)
        {
            tasks[j] = refc<tMyTask>(new tMyTask(kLargeWorkerCountTo));
            keys[j] = pool.push(refc<sync::iRunnable>(tasks[j]));
        }

        for (int j = 0; j < kBatchSize; j++)
        {
            pool.wait(keys[j]);
            t.assert(tasks[j]->didRun());
        }
    }

    t.assert(pool.getNumThreads() == (u32)kNumThreadsInPool);
}


void shortPoolTest(const tTest& t)
{
    int numThreads = (rand() % kMaxThreadsInPool) + 1;
    sync::tThreadPool pool(numThreads);

    t.assert(pool.getNumThreads() == (u32)numThreads);

    int batchSize = (rand() % kMaxBatchSize) + 1;

    for (int i = 0; i < kShortNumTasks; i += batchSize)
    {
        std::vector< refc<tMyTask> > tasks(batchSize);

        std::vector< sync::tThreadPool::tTaskKey > keys(batchSize);

        for (int j = 0; j < batchSize; j++)
        {
            int amountOfWork = (rand() % kSmallWorkerCountTo) + 1;
            tasks[j] = refc<tMyTask>(new tMyTask(amountOfWork));
            keys[j] = pool.push(refc<sync::iRunnable>(tasks[j]));
        }

        for (int j = 0; j < batchSize; j++)
        {
            pool.wait(keys[j]);
            t.assert(tasks[j]->didRun());
        }
    }

    t.assert(pool.getNumThreads() == (u32)numThreads);
}


int main()
{
    tCrashReporter::init();
    srand(time(0));

    tTest("tThreadPool long-worker test", longPoolTest);
    tTest("tThreadPool short-worker test", shortPoolTest, kNumRepeatTests);

    return 0;
}
