#include <rho/ptr.h>
#include <rho/tCrashReporter.h>
#include <rho/tTest.h>
#include <rho/types.h>

#include <cstdlib>
#include <ctime>


using namespace rho;


static i64 gObjectCount = 0;

static i32 gMaxRandTests = 100000;


class tCountingObject
{
    public:

        tCountingObject()
        {
            gObjectCount++;
        }

        tCountingObject(const tCountingObject& other)
        {
            exit(1);
        }

        tCountingObject& operator= (const tCountingObject& other)
        {
            exit(1);
            return *this;
        }

        ~tCountingObject()
        {
            gObjectCount--;
        }

        void foo()
        {
        }

        void bar() const
        {
        }
};


void example1(const tTest& t)
{
    ptr<int> o;
    o = new int(14);
    t.iseq(*o, 14);
    *o = 5;
    t.iseq(*o, 5);
    o = new int(68);
    t.iseq(*o, 68);

    ptr<int> p(new int(47));
    t.iseq(*p, 47);
    *p = 88;
    t.iseq(*p, 88);
    p = new int(71);
    t.iseq(*p, 71);
}

void example2(const tTest& t)
{
    {
        t.iseq(gObjectCount, 0);
        ptr<tCountingObject> p(new tCountingObject);
        t.iseq(gObjectCount, 1);
    }
    t.iseq(gObjectCount, 0);
}

void example3(const tTest& t)
{
    {
        t.iseq(gObjectCount, 0);
        ptr<tCountingObject> p(new tCountingObject);
        t.iseq(gObjectCount, 1);

        p = new tCountingObject;
        t.iseq(gObjectCount, 1);

        p = NULL;
        t.iseq(gObjectCount, 0);
    }
    t.iseq(gObjectCount, 0);
}

void example4(const tTest& t)
{
    {
        ptr<tCountingObject> p;
        p = new tCountingObject;
        t.iseq(gObjectCount, 1);

        (*p).foo();
        p->foo();

        (*p).bar();
        p->bar();
    }
    t.iseq(gObjectCount, 0);

    {
        const ptr<tCountingObject> p(new tCountingObject);
        t.iseq(gObjectCount, 1);

        // foo() isn't const, so I can't call him... :(

        // But I can call bar(), woohoo!
        (*p).bar();
        p->bar();
    }
    t.iseq(gObjectCount, 0);
}

void randRecurse(const tTest& t, double prob, int depth)
{
    ptr<tCountingObject> p;
    p = new tCountingObject;
    t.iseq(depth + 1, gObjectCount);

    if (((double)rand()) / RAND_MAX < prob)
        randRecurse(t, prob / 1.1, depth+1);
}

void randomTest(const tTest& t)
{
    randRecurse(t, 1.0, 0);
}

int main()
{
    tCrashReporter::init();

    tTest("Example 1", example1);
    tTest("Example 2", example2);
    tTest("Example 3", example3);
    tTest("Example 4", example4);

    srand(time(0));
    tTest("Randomized test", randomTest, gMaxRandTests);

    return 0;
}
