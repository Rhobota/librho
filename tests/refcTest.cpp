#include "rho/refc.h"
#include "rho/types.h"

#include <cassert>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <vector>


using namespace rho;
using std::cin;
using std::cout;
using std::endl;
using std::vector;


static i64 gObjectCount = 0;

static const i32 kMaxTests = 1000;


class tCountingObject
{
    public:

        tCountingObject()
        {
            gObjectCount++;
        }

        tCountingObject(const tCountingObject& other)
        {
            assert(false);
        }

        tCountingObject& operator= (const tCountingObject& other)
        {
            assert(false);
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


void randomTest()
{
    // Randomized test 1:
    {
        const int kVectorSize = 100;
        vector< refc<tCountingObject> > v;
        for (int i = 0; i < kVectorSize; i++)
        {
            refc<tCountingObject> r = new tCountingObject;
            v.push_back(r);
            assert(gObjectCount == i+1);
        }
        for (int i = 0; i < 10*kVectorSize; i++)
        {
            int pos1 = rand() % kVectorSize;
            int pos2 = rand() % kVectorSize;
            v[pos1] = v[pos2];
        }
    }
    assert(gObjectCount == 0);

    // Randomized test 2:
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
                    assert(false);
                    break;
                }
            }
        }
    }
    assert(gObjectCount == 0);
}


int main()
{
    // Example:
    {
        refc<int> r = new int(99);
        int x = *r;                  // x == 99
    } // --------------------------> // goes out of scope and is deleted here

    // Example:
    {
        assert(gObjectCount == 0);
        refc<tCountingObject> r = new tCountingObject;
        assert(gObjectCount == 1);

        tCountingObject& c = *r;
        r->foo();
    } // --------------------------> // the tCountingObject is deleted here
    assert(gObjectCount == 0);

    // Example:
    {
        assert(gObjectCount == 0);
        const refc<tCountingObject> r = new tCountingObject;
        assert(gObjectCount == 1);

        const tCountingObject& c = *r;
        r->bar();
    }
    assert(gObjectCount == 0);

    // Example:
    {
        refc<int> myref = new int(15);
        refc<int> myref2 = myref;
        refc<int> myref3 = new int(48);

        assert(myref == myref2);
        assert(myref != myref3);
        assert(myref2 != myref3);
        assert(!(myref < myref2));
    }

    // Example:
    {
        tCountingObject* object = new tCountingObject;
        refc<tCountingObject> myref(object);
        assert(gObjectCount == 1);

        {
            refc<tCountingObject> myref2(new tCountingObject);
            assert(gObjectCount == 2);
        }
        assert(gObjectCount == 1);

        {
            refc<tCountingObject> myref2(object);
            assert(gObjectCount == 1);     // because 'object' is already refc'd
        }
        assert(gObjectCount == 1);
    }
    assert(gObjectCount == 0);

    // Example:
    {
        try
        {
            refc<tCountingObject> r(NULL);
            assert(false);
        }
        catch (std::logic_error& e)
        {
        }

        try
        {
            refc<tCountingObject> r(new tCountingObject);
            assert(gObjectCount == 1);
            r = NULL;
            assert(false);
        }
        catch (std::logic_error& e)
        {
        }
        assert(gObjectCount == 0);
    }

    // Test:
    {
        refc<tCountingObject> myref = new tCountingObject;
        refc<tCountingObject> myref2 = new tCountingObject;
        assert(gObjectCount == 2);
        myref = myref2;
        assert(gObjectCount == 1);
    }
    assert(gObjectCount == 0);

    // Test:
    {
        refc<tCountingObject> myref = new tCountingObject;
        refc<tCountingObject> myref2 = myref;
        assert(gObjectCount == 1);
        {
            refc<tCountingObject> myref3 = myref2;
            assert(gObjectCount == 1);
        }
        assert(gObjectCount == 1);
    }
    assert(gObjectCount == 0);

    // Test:
    {
        refc<tCountingObject> myref(new tCountingObject);
        refc<tCountingObject> myref2(myref);
        assert(gObjectCount == 1);
        {
            refc<tCountingObject> myref3(myref2);
            refc<tCountingObject> myref4(myref);
            assert(gObjectCount == 1);
        }
        assert(gObjectCount == 1);
    }
    assert(gObjectCount == 0);

    // Many randomized tests:
    srand(time(0));
    for (int i = 0; i < kMaxTests; i++)
    {
        randomTest();
    }

    return 0;
}
