#include <rho/types.h>
#include <rho/tTest.h>
#include <rho/tCrashReporter.h>

using namespace rho;


void typesTest(const tTest& t)
{
    t.assert(sizeof(u8) == 1);
    t.assert(sizeof(i8) == 1);
    t.assert(sizeof(u16) == 2);
    t.assert(sizeof(i16) == 2);
    t.assert(sizeof(u32) == 4);
    t.assert(sizeof(i32) == 4);
    t.assert(sizeof(u64) == 8);
    t.assert(sizeof(i64) == 8);
}


int main()
{
    tCrashReporter::init();

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

    tTest("types test", typesTest);

    return 0;
}
