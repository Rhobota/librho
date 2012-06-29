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

static const i32 kMaxTests = 100000;


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
};


void randomTest()
{
    {
        const int kVectorSize = 1000;
        std::vector< refc<tCountingObject> > v;
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
}


int main()
{
    {
        refc<int> r = new int(99);
        int x = *r;                  // x == 99
    } // --------------------------> // goes out of scope and is deleted here

    {
        assert(gObjectCount == 0);
        refc<tCountingObject> r = new tCountingObject;
        assert(gObjectCount == 1);
        r->foo();                    // cool huh?
    } // --------------------------> // the tCountingObject is deleted here
    assert(gObjectCount == 0); // -> // proof the delete happened

    {
        refc<tCountingObject> myref = new tCountingObject;
        refc<tCountingObject> myref2 = new tCountingObject;
        assert(gObjectCount == 2);
        myref = myref2;
        assert(gObjectCount == 1);
    }
    assert(gObjectCount == 0);

    {
        refc<tCountingObject> myref = new tCountingObject;
        refc<tCountingObject> myref2 = myref;
        assert(gObjectCount == 1);
    }
    assert(gObjectCount == 0);

    {
        refc<tCountingObject> myref(new tCountingObject);
        refc<tCountingObject> myref2(myref);
        assert(gObjectCount == 1);
    }
    assert(gObjectCount == 0);

    srand(time(0));
    for (int i = 0; i < kMaxTests; i++)
    {
        randomTest();
    }

    return 0;
}
