#include <rho/sync/iSync.h>
#include <rho/tTest.h>


using namespace rho;


int gCallCount = 0;


class tFoo : public sync::iSync
{
    public:

        void acquire() const
        {
            ++gCallCount;
        }

        void release() const
        {
            ++gCallCount;
        }
};


void test(const tTest& t)
{
    sync::iSync* s = new tFoo;
    s->acquire();             // tests to be sure acquire() is declared virtual
    s->release();             // tests to be sure release() is declared virtual
    t.iseq(gCallCount, 2);
}


int main()
{
    tTest("iSync test", test);

    return 0;
}
