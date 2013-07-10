#include <rho/sync/tThreadPool.h>
#include <rho/bNonCopyable.h>
#include <rho/tCrashReporter.h>
#include <rho/tTest.h>


using namespace rho;


const int kNumThreadsInPool = 32;
const int kNumTasks = 10000;
const int kBatchSize = 200;
const int kWorkerCountTo = 10000000;


class tMyTask : public sync::iRunnable, public bNonCopyable
{
    public:

        tMyTask()
            : m_answer(0),
              m_threadDidRunYay(false)
        {
        }

        void run()
        {
            int answer = 0;
            for (int i = 0; i < kWorkerCountTo; i++)
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

        int m_answer;
        bool m_threadDidRunYay;
};


void poolTest(const tTest& t)
{
    sync::tThreadPool pool(kNumThreadsInPool);

    t.assert(pool.getNumThreads() == (u32)kNumThreadsInPool);

    for (int i = 0; i < kNumTasks; i += kBatchSize)
    {
        std::vector< refc<tMyTask> >  tasks(kBatchSize);
        std::vector< sync::tTaskKey > keys(kBatchSize);

        for (int j = 0; j < kBatchSize; j++)
        {
            tasks[j] = refc<tMyTask>(new tMyTask);
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


int main()
{
    tCrashReporter::init();

    tTest("tThreadPool test", poolTest);

    return 0;
}
