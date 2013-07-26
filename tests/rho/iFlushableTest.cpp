#include <rho/iFlushable.h>
#include <rho/tCrashReporter.h>
#include <rho/tTest.h>


using namespace rho;


int gItWorked = 0;


class tFoo : public iFlushable
{
    public:

        bool flush()
        {
            gItWorked++;
            return true;
        }

        ~tFoo()
        {
            gItWorked++;
        }
};


void test(const tTest& t)
{
    iFlushable* p = new tFoo();
    p->flush();                       // makes sure flush() is virtual
    t.assert(gItWorked == 1);
    delete p;                         // makes sure the d'tor is virtual
    t.assert(gItWorked == 2);
}


int main()
{
    tCrashReporter::init();

    tTest("iFlushable test", test);

    return 0;
}
