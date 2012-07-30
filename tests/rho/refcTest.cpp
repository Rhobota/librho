#include <rho/refc.h>
#include <rho/tCrashReporter.h>
#include <rho/tTest.h>
#include <rho/types.h>

#include <rho/sync/tThread.h>
#include <rho/sync/tThreadLocal.h>

#include <cstdlib>
#include <ctime>
#include <iostream>
#include <vector>


using namespace rho;
using std::cout;
using std::endl;
using std::vector;


static const i32 kMaxTests = 100;
static const i32 kMaxThreads = 100;


static i64 gObjectCount = 0;


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


static sync::tThreadLocal<i64> gThreadLocalObjectCount;


class tThreadLocalCountingObject
{
    public:

        tThreadLocalCountingObject()
        {
            (*gThreadLocalObjectCount)++;
        }

        tThreadLocalCountingObject(const tThreadLocalCountingObject& other)
        {
            exit(1);
        }

        tThreadLocalCountingObject& operator= (
                const tThreadLocalCountingObject& other)
        {
            exit(1);
            return *this;
        }

        ~tThreadLocalCountingObject()
        {
            (*gThreadLocalObjectCount)--;
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
    refc<int> r = new int(99);
    int x = *r;                  // x == 99
    t.iseq(x, 99);
} // --------------------------> // The 'int' is delete when 'r' goes oos here.


void example2(const tTest& t)
{
    {
        t.assert(gObjectCount == 0);
        refc<tCountingObject> r = new tCountingObject;
        t.assert(gObjectCount == 1);

        tCountingObject& c = *r;
        r->foo();
        c.foo();
    } // --------------------------> // the tCountingObject is deleted here
    t.assert(gObjectCount == 0);
}


void example3(const tTest& t)
{
    {
        t.assert(gObjectCount == 0);
        const refc<tCountingObject> r = new tCountingObject;
        t.assert(gObjectCount == 1);

        const tCountingObject& c = *r;
        r->bar();
        c.bar();
    }
    t.assert(gObjectCount == 0);
}


void example4(const tTest& t)
{
    {
        refc<int> myref = new int(15);
        refc<int> myref2 = myref;
        refc<int> myref3 = new int(48);

        t.assert(myref == myref2);
        t.assert(myref != myref3);
        t.assert(myref2 != myref3);
        t.assert(!(myref < myref2));
    }
}


void example5(const tTest& t)
{
    {
        tCountingObject* object = new tCountingObject;
        refc<tCountingObject> myref(object);
        t.assert(gObjectCount == 1);

        {
            refc<tCountingObject> myref2(new tCountingObject);
            t.assert(gObjectCount == 2);
        }
        t.assert(gObjectCount == 1);

        {
            refc<tCountingObject> myref2(object);
            t.assert(gObjectCount == 1);   // because 'object' is already refc'd
        }
        t.assert(gObjectCount == 1);
    }
    t.assert(gObjectCount == 0);
}


void example6(const tTest& t)
{
    {
        try
        {
            refc<tCountingObject> r(NULL);
            t.assert(false);
        }
        catch (eNullPointer& e)
        {
        }

        try
        {
            refc<tCountingObject> r(new tCountingObject);
            t.assert(gObjectCount == 1);
            r = NULL;
            t.assert(false);
        }
        catch (eNullPointer& e)
        {
        }
        t.assert(gObjectCount == 0);
    }
}


void test1(const tTest& t)
{
    {
        refc<tCountingObject> myref = new tCountingObject;
        refc<tCountingObject> myref2 = new tCountingObject;
        t.assert(gObjectCount == 2);
        myref = myref2;
        t.assert(gObjectCount == 1);
    }
    t.assert(gObjectCount == 0);
}


void test2(const tTest& t)
{
    {
        refc<tCountingObject> myref = new tCountingObject;
        refc<tCountingObject> myref2 = myref;
        t.assert(gObjectCount == 1);
        {
            refc<tCountingObject> myref3 = myref2;
            t.assert(gObjectCount == 1);
        }
        t.assert(gObjectCount == 1);
    }
    t.assert(gObjectCount == 0);
}


void test3(const tTest& t)
{
    {
        refc<tCountingObject> myref(new tCountingObject);
        refc<tCountingObject> myref2(myref);
        t.assert(gObjectCount == 1);
        {
            refc<tCountingObject> myref3(myref2);
            refc<tCountingObject> myref4(myref);
            t.assert(gObjectCount == 1);
        }
        t.assert(gObjectCount == 1);
    }
    t.assert(gObjectCount == 0);
}


void randomTest1(const tTest& t)
{
    {
        const int kVectorSize = 100;
        vector< refc<tCountingObject> > v;
        for (int i = 0; i < kVectorSize; i++)
        {
            refc<tCountingObject> r = new tCountingObject;
            v.push_back(r);
            t.assert(gObjectCount == i+1);
        }
        for (int i = 0; i < 10*kVectorSize; i++)
        {
            int pos1 = rand() % kVectorSize;
            int pos2 = rand() % kVectorSize;
            v[pos1] = v[pos2];
        }
    }
    t.assert(gObjectCount == 0);
}


void randomTest2(const tTest& t)
{
    {
        const int kVectorSize = 100;
        vector< refc<tCountingObject> > v;
        for (int i = 0; i < kVectorSize; i++)
        {
            v.push_back(refc<tCountingObject>(new tCountingObject));
        }

        for (int i = 0; i < 100*kVectorSize; i++)
        {
            int pos1 = rand() % kVectorSize;
            int pos2 = rand() % kVectorSize;
            int op   = rand() % 6;
            switch (op)
            {
                case 0:
                {
                    // Tests operator=(T*) with a new object.
                    v[pos1] = new tCountingObject;
                    break;
                }
                case 1:
                {
                    // Tests operator=(T*) with a per-existing object.
                    tCountingObject& o = *v[pos2];
                    v[pos1] = &o;
                    break;
                }
                case 2:
                {
                    // Tests refc(T*) and operator=(refc&) with a new object.
                    refc<tCountingObject> r(new tCountingObject);
                    v[pos1] = r;
                    break;
                }
                case 3:
                {
                    // Tests refc(T*) and operator=(refc&) with a pre-ex object.
                    tCountingObject& o = *v[pos2];
                    refc<tCountingObject> r(&o);
                    v[pos1] = r;
                    break;
                }
                case 4:
                {
                    // Tests operator=(refc&).
                    v[pos1] = v[pos2];
                    break;
                }
                case 5:
                {
                    // Tests refc(refc&).
                    refc<tCountingObject> r1(v[pos1]);
                    refc<tCountingObject> r2(v[pos2]);
                    break;
                }
                default:
                {
                    t.assert(false);
                    break;
                }
            }
        }
    }
    t.assert(gObjectCount == 0);
}


void threadLocalRandomTest2(const tTest& t)
{
    {
        const int kVectorSize = 100;
        vector< refc<tThreadLocalCountingObject> > v;
        for (int i = 0; i < kVectorSize; i++)
        {
            v.push_back(refc<tThreadLocalCountingObject>(
                                       new tThreadLocalCountingObject));
        }

        for (int i = 0; i < 100*kVectorSize; i++)
        {
            int pos1 = rand() % kVectorSize;
            int pos2 = rand() % kVectorSize;
            int op   = rand() % 6;
            switch (op)
            {
                case 0:
                {
                    // Tests operator=(T*) with a new object.
                    v[pos1] = new tThreadLocalCountingObject;
                    break;
                }
                case 1:
                {
                    // Tests operator=(T*) with a per-existing object.
                    tThreadLocalCountingObject& o = *v[pos2];
                    v[pos1] = &o;
                    break;
                }
                case 2:
                {
                    // Tests refc(T*) and operator=(refc&) with a new object.
                    refc<tThreadLocalCountingObject> r(
                                          new tThreadLocalCountingObject);
                    v[pos1] = r;
                    break;
                }
                case 3:
                {
                    // Tests refc(T*) and operator=(refc&) with a pre-ex object.
                    tThreadLocalCountingObject& o = *v[pos2];
                    refc<tThreadLocalCountingObject> r(&o);
                    v[pos1] = r;
                    break;
                }
                case 4:
                {
                    // Tests operator=(refc&).
                    v[pos1] = v[pos2];
                    break;
                }
                case 5:
                {
                    // Tests refc(refc&).
                    refc<tThreadLocalCountingObject> r1(v[pos1]);
                    refc<tThreadLocalCountingObject> r2(v[pos2]);
                    break;
                }
                default:
                {
                    t.assert(false);
                    break;
                }
            }
        }
    }
    t.assert(*gThreadLocalObjectCount == 0);
}


class tRefcTestRunnable : public sync::iRunnable
{
    public:

        tRefcTestRunnable(const tTest& t) : m_t(t) { }

        void run()
        {
            gThreadLocalObjectCount = new i64(0);
            threadLocalRandomTest2(m_t);
            gThreadLocalObjectCount = NULL;
        }

    private:

        const tTest& m_t;
};


void threadTest(const tTest& t)
{
    int numThreads = rand() % kMaxThreads + 1;
    vector< refc<sync::tThread> > threads;
    for (int i = 0; i < numThreads; i++)
        threads.push_back(new sync::tThread(new tRefcTestRunnable(t)));
    for (int i = 0; i < numThreads; i++)
        threads[i]->join();
}


int main()
{
    tCrashReporter::init();

    tTest("Example 1", example1);
    tTest("Example 2", example2);
    tTest("Example 3", example3);
    tTest("Example 4", example4);
    tTest("Example 5", example5);
    tTest("Example 6", example6);

    tTest("Test 1", test1);
    tTest("Test 2", test2);
    tTest("Test 3", test3);

    srand(time(0));
    tTest("Randomized test 1", randomTest1, kMaxTests);
    tTest("Randomized test 2", randomTest2, kMaxTests);

    tTest("Randomized test 2 (with threads)", threadTest, kMaxTests/10);

    return 0;
}
