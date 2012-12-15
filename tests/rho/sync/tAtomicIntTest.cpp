#include <rho/sync/tAtomicInt.h>
#include <rho/sync/tThread.h>
#include <rho/tCrashReporter.h>
#include <rho/tTest.h>

#include <cstdlib>
#include <ctime>
#include <iostream>
#include <vector>


using namespace rho;
using std::cerr;
using std::endl;


const int kTestCount = 200;

sync::ai32 gCount;

sync::ai32 gGotItCount;


class tFoo : public sync::iRunnable
{
    public:

        void run()
        {
            while (true)
            {
                i32 localCount = --gCount;
                if (localCount == 0)
                    ++gGotItCount;
                else if (localCount < 0)
                    break;
            }
        }
};


void test(const tTest& t)
{
    int numThreads = rand() % 100 + 1;
    gCount = rand() % 100000 + 1;
    gGotItCount = 0;

    std::vector< refc<sync::tThread> > threads;
    for (int i = 0; i < numThreads; i++)
        threads.push_back(refc<sync::tThread>(new sync::tThread(refc<sync::iRunnable>(new tFoo))));

    for (int i = 0; i < numThreads; i++)
        threads[i]->join();

    try
    {
        t.iseq(gGotItCount.val(), 1);
    }
    catch (tTest::eTestFailedError& e)
    {
        cerr << gGotItCount << " != 1" << endl;
        throw;
    }
}


int main()
{
    tCrashReporter::init();

    srand(time(0));

    tTest("tAtomicInt test", test, kTestCount);

    return 0;
}
