#include "rho/sync/tMutex.h"
#include "rho/sync/tThread.h"
#include "rho/tCrashReporter.h"
#include "rho/tTest.h"
#include "rho/types.h"

#include <iostream>
#include <vector>


using namespace rho;
using std::cout;
using std::endl;


// i32 kNumThreads = 10;
// i32 kCountMax = 10000;
// u64 gCount = 0;

i32 kNumThreads = 10;
i32 kCountMax = 10;
u64 gCount = 0;


class tCounter : public sync::iRunnable
{
    public:

        tCounter(sync::tMutex* mutex)
            : m_mutex(mutex)
        {
        }

        void run()
        {
            for (int i = 0; i < kCountMax; i++)
            {
                m_mutex->acquire();
                u64 currCount = gCount;
                currCount += 1;
                gCount = currCount;
                m_mutex->release();
            }
        }

    private:

        sync::tMutex* m_mutex;
};


void test(const tTest& t)
{
    sync::tMutex mutex;

    std::vector< refc<sync::tThread> > threads;

    for (int i = 0; i < kNumThreads; i++)
    {
        tCounter* counter = new tCounter(&mutex);
        sync::tThread* thread = new sync::tThread(counter);
        threads.push_back(thread);
    }

    for (int i = 0; i < kNumThreads; i++)
    {
        threads[i]->join();
    }

    cout << gCount << endl;
    t.iseq(gCount, kNumThreads * kCountMax);
}


int main()
{
    tCrashReporter::init();

    while (true)
    {
        gCount = 0;
        tTest("Mutex test", test);
    }

    return 0;
}
