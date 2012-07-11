#include "rho/bNonCopyable.h"


/*
 * This is really just a compilation test; it makes sure you can #include
 * the file and derive a class from bNonCopyable.
 *
 * This test is extremely crappy because the purpose of bNonCopyable is to
 * prevent code from compiling when a non-copyable object's copy constructor
 * or operator= is called from anywhere.
 *
 * What we really need to test this is a series of programs that should *not*
 * compile, but we don't have the testing infrastructure to do that right now.
 */


class tFoo : public rho::bNonCopyable
{
    public:

        tFoo()
        {
        }

        void foo()
        {
        }
};


int main()
{
    tFoo f;

    f.foo();

    // The following shouldn't compile.
    // tFoo z(f);
    // f = tFoo();

    return 0;
}
