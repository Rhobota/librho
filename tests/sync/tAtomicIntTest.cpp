#include "rho/sync/tAtomicInt.h"
#include "rho/sync/tThread.h"
#include "rho/tCrashReporter.h"
#include "rho/tTest.h"

#include <cstdlib>
#include <ctime>
#include <iostream>
#include <vector>


using namespace rho;
using std::cout;
using std::endl;


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
    while (true)
    {
        int numThreads = rand() % 100 + 1;
        gCount = rand() % 100000 + 1;
        gGotItCount = 0;

        std::vector< refc<sync::tThread> > threads;
        for (int i = 0; i < numThreads; i++)
            threads.push_back(new sync::tThread(new tFoo));

        for (int i = 0; i < numThreads; i++)
            threads[i]->join();

        try
        {
            t.iseq((i32)gGotItCount, 1);
        }
        catch (tTest::eTestFailedError& e)
        {
            cout << gGotItCount << " != 1" << endl;
            throw;
        }
    }
}


int main()
{
    tCrashReporter::init();

    srand(time(0));

    tTest("tAtomicInt test", test);

    return 0;
}
