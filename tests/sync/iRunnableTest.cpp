#include "rho/sync/iRunnable.h"
#include "rho/tTest.h"


using namespace rho;


bool gWorked = false;


class tFoo : public sync::iRunnable
{
    public:

        void run()
        {
            gWorked = true;
        }
};


void test(const tTest& t)
{
    sync::iRunnable* r = new tFoo;
    r->run();                      // tests to be sure run() is declared virtual
    t.assert(gWorked);
}


int main()
{
    tTest("iRunnable test", test);

    return 0;
}
