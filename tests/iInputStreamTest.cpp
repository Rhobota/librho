#include "rho/iInputStream.h"


/*
 * Just a compilation test. (An interface can't be tested more than that.)
 */


class tFoo : public rho::iInputStream
{
    public:

        int read(rho::u8* buffer, int length) { }

        void foo() { }
};


int main()
{
    tFoo f;

    f.foo();

    return 0;
}
