#include "rho/iInputStream.h"
#include "rho/tCrashReporter.h"
#include "rho/tTest.h"


using namespace rho;


bool gItWorked = false;


class tFoo : public iInputStream
{
    public:

        int read(u8* buffer, int length)
        {
            gItWorked = true;
            return 0;
        }
};


void test(const tTest& t)
{
    iInputStream* p = new tFoo();
    p->read(NULL, 78);            // This only tests if the method is virtual...
    t.assert(gItWorked);
    delete p;
}


int main()
{
    tCrashReporter::init();

    tTest("iInputStream test", test);

    return 0;
}
