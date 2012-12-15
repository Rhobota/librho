#include <rho/sync/tThread.h>
#include <rho/tCrashReporter.h>
#include <rho/tTest.h>

#include <iostream>


using namespace rho;
using std::cout;
using std::endl;


const int kNumTestIterations = 10000;

bool gThreadDidRunYay = false;


class tFoo : public sync::iRunnable
{
    public:

        void run()
        {
            gThreadDidRunYay = true;
        }
};


void test1(const tTest& t)
{
    gThreadDidRunYay = false;

    sync::tThread thread(refc<sync::iRunnable>(new tFoo));
    thread.join();

    t.iseq(gThreadDidRunYay, true);
}


int main()
{
    tCrashReporter::init();

    tTest("tThread test 1", test1, kNumTestIterations);

    return 0;
}
