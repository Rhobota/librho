#include "rho/sync/tThread.h"
#include "rho/tCrashReporter.h"
#include "rho/tTest.h"

#include <iostream>


using namespace rho;
using std::cout;
using std::endl;


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
    t.iseq(gThreadDidRunYay, false);

    tFoo foo;
    sync::tThread thread(&foo);
    thread.join();

    t.iseq(gThreadDidRunYay, true);
}


int main()
{
    tCrashReporter::init();

    tTest("Thread test 1", test1);

    return 0;
}
