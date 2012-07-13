#include "rho/ptr.h"
#include "rho/types.h"

#include <cassert>
#include <iostream>


using namespace rho;
using std::cout;
using std::endl;


/*
 * This will be used with the tCountingObject class to determine how
 * many objects are in memory at any given time.
 */
static i64 gObjectCount = 0;


/*
 * This class counts the above variable up when it is instantiated, and counts
 * it down when an instance destructs.
 */
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


void example1()
{
    ptr<int> o;
    o = new int(14);
    assert(*o == 14);
    *o = 5;
    assert(*o == 5);
    o = new int(68);        // No need to delete the old int; it's done for you.
    assert(*o == 68);

    ptr<int> p(new int(47));
    assert(*p == 47);
    *p = 88;
    assert(*p == 88);
    p = new int(71);
    assert(*p == 71);
}                           // The outstanding ints will be deleted here.

void example2()
{
    {
        assert(gObjectCount == 0);
        ptr<tCountingObject> p(new tCountingObject);
        assert(gObjectCount == 1);
    }
    assert(gObjectCount == 0);        // <--- proof of the delete. See?
}

void example3()
{
    {
        assert(gObjectCount == 0);
        ptr<tCountingObject> p(new tCountingObject);
        assert(gObjectCount == 1);

        p = new tCountingObject;      // You don't have to call delete yourself!
        assert(gObjectCount == 1);

        p = NULL;                     // You can force a delete like this.
        assert(gObjectCount == 0);
    }
    assert(gObjectCount == 0);
}

void example4()
{
    // Non 'const' example:
    {
        ptr<tCountingObject> p;
        p = new tCountingObject;

        (*p).foo();               // Make calls either...
        p->foo();                 // ... way.

        (*p).bar();
        p->bar();
    }

    // 'Const' example:
    {
        const ptr<tCountingObject> p(new tCountingObject);

        // foo() isn't const, so I can't call him... :(

        // But I can call bar(), woohoo!
        (*p).bar();
        p->bar();
    }
}

int main()
{
    example1();
    example2();
    example3();
    example4();

    cout << "This example is all about memory allocation..." << endl;
    cout << "... so you'll have to acutally look at the code." << endl;

    return 0;
}
