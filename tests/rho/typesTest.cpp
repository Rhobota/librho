#include "rho/types.h"


/*
 * This is just a compilation test.
 */


using namespace rho;


int main()
{
    u8  a;
    u16 b;
    u32 c;
    u64 d;

    i8  e;
    i16 f;
    i32 g;
    i64 h;

    (a + b + c + d +
     e + f + g + h);       // This is just to avoid the "unsued var" error

    return 0;
}
