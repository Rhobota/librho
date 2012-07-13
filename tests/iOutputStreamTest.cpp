#include "rho/iOutputStream.h"
#include "rho/tCrashReporter.h"
#include "rho/tTest.h"


using namespace rho;


bool gItWorked = false;


class tFoo : public iOutputStream
{
    public:

        int write(const u8* buffer, int length)
        {
            gItWorked = true;
            return 0;
        }
};


void test(const tTest& t)
{
    iOutputStream* p = new tFoo();
    p->write(NULL, 29);     // Makes sure the method is virtual.
    t.assert(gItWorked);
    delete p;
}


int main()
{
    tCrashReporter::init();

    tTest("iOutputStream test", test);

    return 0;
}
