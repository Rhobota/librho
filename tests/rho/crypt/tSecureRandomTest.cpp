#include <rho/crypt/tSecureRandom.h>
#include <rho/tCrashReporter.h>
#include <rho/tTest.h>

#include <iostream>
#include <vector>


using namespace rho;
using std::cout;
using std::endl;
using std::vector;


static const int kRandBufSize = 16;


bool same(u8* buf1, u8* buf2, int size)
{
    for (int i = 0; i < size; i++)
        if (buf1[i] != buf2[i])
            return false;
    return true;
}


void test(const tTest& t)
{
    crypt::tSecureRandom sr;

    u8 buf1[kRandBufSize];
    u8 buf2[kRandBufSize];

    i32 r1 = sr.read(buf1, kRandBufSize);
    i32 r2 = sr.read(buf2, kRandBufSize);

    t.assert(r1 == kRandBufSize);
    t.assert(r2 == kRandBufSize);

    t.assert(! same(buf1, buf2, kRandBufSize));
}


int main()
{
    tCrashReporter::init();

    tTest("tSecureRandom test", test);

    return 0;
}
