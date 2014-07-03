#include <rho/sync/tThreadLocal.h>
#include <rho/sync/tThread.h>
#include <rho/tCrashReporter.h>
#include <rho/tTest.h>

#include <cstdlib>
#include <ctime>
#include <vector>


using namespace rho;
using std::vector;


const int kTestIterations = 2;
const int kMaxThreads = 100;
const int kThreadIterations = 1000;

sync::tThreadLocal<int> gGlobalInt1;
sync::tThreadLocal<int> gGlobalInt2;
sync::tThreadLocal<int> gGlobalInt3;
sync::tThreadLocal<int> gGlobalInt4;


class tFoo : public sync::iRunnable
{
    public:

        tFoo(const tTest& t) : m_t(t) { }

        void run()
        {
            int localInt1 = rand() % 10;
            int localInt2 = rand() % 10;
            int localInt3 = rand() % 10;
            int localInt4 = rand() % 10;

            gGlobalInt1 = new int(localInt1);
            gGlobalInt2 = new int(localInt2);
            gGlobalInt3 = new int(localInt3);
            gGlobalInt4 = new int(localInt4);

            for (int i = 0; i < kThreadIterations; i++)
            {
                m_t.iseq(localInt1, *gGlobalInt1);
                m_t.iseq(localInt2, *gGlobalInt2);
                m_t.iseq(localInt3, *gGlobalInt3);
                m_t.iseq(localInt4, *gGlobalInt4);

                *gGlobalInt1 = localInt1 = rand() % 10;
                *gGlobalInt2 = localInt2 = rand() % 10;
                *gGlobalInt3 = localInt3 = rand() % 10;
                *gGlobalInt4 = localInt4 = rand() % 10;
            }

            gGlobalInt1 = NULL;
            gGlobalInt2 = NULL;
            gGlobalInt3 = NULL;
            gGlobalInt4 = NULL;
        }

    private:

        const tTest& m_t;
};


void test(const tTest& t)
{
    int numThreads = rand() % kMaxThreads + 1;

    vector< refc<sync::tThread> > threads;

    for (int i = 0; i < numThreads; i++)
        threads.push_back(refc<sync::tThread>(new sync::tThread(refc<sync::iRunnable>(new tFoo(t)))));

    for (int i = 0; i < numThreads; i++)
        threads[i]->join();
}


int main()
{
    tCrashReporter::init();

    srand((u32)time(0));
    tTest("Thread Local Storage test", test, kTestIterations);

    return 0;
}
