#include <rho/sync/tThread.h>
#include <rho/bNonCopyable.h>
#include <rho/tCrashReporter.h>
#include <rho/tTest.h>

#include <vector>


using namespace rho;


const int kNumSingleThreads = 10000;
const int kNumConcurrentThreads = 200;
const int kWorkload = 1000000;


class tFoo : public sync::iRunnable, public bNonCopyable
{
    public:

        tFoo(bool doWork)
            : m_threadDidRunYay(false), m_doWork(doWork)
        {
        }

        void run()
        {
            if (m_doWork)
            {
                int answer = 0;
                for (int i = 0; i < kWorkload; i++)
                    answer *= (i+4)-i*i;
            }
            m_threadDidRunYay = true;
        }

        bool didRun()
        {
            return m_threadDidRunYay;
        }

    private:

        bool m_threadDidRunYay;
        bool m_doWork;
};


void test1(const tTest& t)
{
    refc<tFoo> foo(new tFoo(false));
    refc<sync::iRunnable> runnable(foo);

    sync::tThread thread(runnable);
    thread.join();

    t.assert(foo->didRun());
}


void test2(const tTest& t)
{
    std::vector< refc<tFoo> > foos;
    std::vector< refc<sync::tThread> > threads;

    for (int i = 0; i < kNumConcurrentThreads; i++)
    {
        refc<tFoo> foo(new tFoo(true));
        foos.push_back(foo);

        refc<sync::tThread> thread(new sync::tThread(refc<sync::iRunnable>(foo)));
        threads.push_back(thread);
    }

    for (size_t i = 0; i < threads.size(); i++)
    {
        threads[i]->join();
    }

    t.assert((int)foos.size() == kNumConcurrentThreads);
    for (size_t i = 0; i < foos.size(); i++)
    {
        t.assert(foos[i]->didRun());
    }
}


int main()
{
    tCrashReporter::init();

    tTest("Single-thread test", test1, kNumSingleThreads);
    tTest("Concurrent test", test2);

    return 0;
}
