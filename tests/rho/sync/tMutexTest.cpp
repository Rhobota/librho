#include <rho/sync/tMutex.h>
#include <rho/sync/tThread.h>
#include <rho/tCrashReporter.h>
#include <rho/tTest.h>
#include <rho/types.h>

#include <cstdlib>
#include <ctime>
#include <vector>


using namespace rho;


const i32 kTestIterations = 30;

const i32 kMaxThreads = 100;
const i64 kMaxCount = 10000;

i64 gCount = 0;


class tCounter : public sync::iRunnable
{
    public:

        tCounter(sync::tMutex* mutex, i64 count)
            : m_mutex(mutex),
              m_count(count)
        {
        }

        void run()
        {
            for (int i = 0; i < m_count; i++)
            {
                m_mutex->acquire();
                i64 currCount = gCount;
                currCount += 1;
                gCount = currCount;
                m_mutex->release();
            }
        }

    private:

        sync::tMutex* m_mutex;
        i64           m_count;
};


void test(const tTest& t)
{
    sync::tMutex mutex;

    gCount = 0;

    i32 numThreads = rand() % kMaxThreads + 1;
    i64 count = rand() % kMaxCount + 1;

    i64 expectedEndCount = numThreads * count;

    std::vector< refc<sync::tThread> > threads;

    for (int i = 0; i < numThreads; i++)
    {
        tCounter* counter = new tCounter(&mutex, count);
        sync::tThread* thread = new sync::tThread(counter);
        threads.push_back(thread);
    }

    for (int i = 0; i < numThreads; i++)
    {
        threads[i]->join();
    }

    t.iseq(gCount, expectedEndCount);
}


int main()
{
    tCrashReporter::init();

    srand(time(0));

    tTest("tMutex test", test, kTestIterations);

    return 0;
}
