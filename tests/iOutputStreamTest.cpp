#include "rho/iOutputStream.h"


/*
 * Just a compilation test. (An interface can't be tested more than that.)
 */


class tFoo : public rho::iOutputStream
{
    public:

        int write(const rho::u8* buffer, int length) { }

        void foo() { }
};


int main()
{
    tFoo f;

    f.foo();

    return 0;
}
