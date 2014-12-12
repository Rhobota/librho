#include <rho/refc.h>
#include <rho/types.h>

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
            cout << "New object!" << endl;
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
            cout << "Dead object..." << endl;
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
    refc<int> r;
    r = new int(99);
    int x = *r;
    assert(x == 99);
} // --------------------------> // The 'int' is delete when 'r' goes oos here.


void example2()
{
    {
        assert(gObjectCount == 0);
        refc<tCountingObject> r; r = new tCountingObject;
        assert(gObjectCount == 1);

        tCountingObject& c = *r;     // cool huh?
        r->foo();                    // also cool!
        c.foo();
    } // --------------------------> // the tCountingObject is deleted here

    assert(gObjectCount == 0);       // <---- proof of the deletion
}


void example3()
{
    // 'Const' example:
    {
        assert(gObjectCount == 0);
        const refc<tCountingObject> r(new tCountingObject);
        assert(gObjectCount == 1);

        // Can't call r->foo() because foo() is not labels 'const'

        // But bar() is labeled 'const', so we can call that!
        const tCountingObject& c = *r;
        r->bar();
        c.bar();
    }

    assert(gObjectCount == 0);
}


void example4()
{
    {
        refc<int> myref(new int(15));
        refc<int> myref2(myref);
        refc<int> myref3(new int(15));

        assert(myref == myref2);          //
        assert(myref != myref3);          // These operators operator on the ref
        assert(myref2 != myref3);         // itself, not on the thing inside.
        assert(!(myref < myref2));        //
    }
}


void example5()
{
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
            assert(gObjectCount == 1);   // because 'object' is already refc'd
                                         // (refc is very smart -- it'd be hard
                                         // to use it incorrectly)
        }
        assert(gObjectCount == 1);
    }
    assert(gObjectCount == 0);
}


int main()
{
    example1();
    example2();
    example3();
    example4();
    example5();

    cout << "This example is all about memory allocation..." << endl;
    cout << "... so you'll have to acutally look at the code." << endl;

    return 0;
}
