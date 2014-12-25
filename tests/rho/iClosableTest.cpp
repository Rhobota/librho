#include <rho/iClosable.h>
#include <rho/tCrashReporter.h>
#include <rho/tTest.h>


using namespace rho;


int gItWorked = 0;


class tFoo : public iClosable
{
    public:

        void close()
        {
            gItWorked++;
        }

        ~tFoo()
        {
            gItWorked++;
        }
};


void test(const tTest& t)
{
    iClosable* p = new tFoo();
    p->close();                       // makes sure close() is virtual
    t.assert(gItWorked == 1);
    delete p;                         // makes sure the d'tor is virtual
    t.assert(gItWorked == 2);
}


int main()
{
    tCrashReporter::init();

    tTest("iClosable test", test);

    return 0;
}
