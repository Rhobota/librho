#include <rho/crypt/tSecureRandom.h>
#include <rho/tCrashReporter.h>
#include <rho/tTest.h>

#include <iostream>
#include <set>
#include <vector>

using namespace rho;
using std::cout;
using std::endl;
using std::set;
using std::vector;


static const int kNumTests = 2;
static const int kRandBufSize = 16;   // <-- like a GUID
static const int kNumBufs = 1000000;


void test(const tTest& t)
{
    set< vector<u8> > bufs;

    crypt::tSecureRandom sr1;
    crypt::tSecureRandom sr2;

    for (int i = 0; i < kNumBufs; i++)
    {
        vector<u8> b1(kRandBufSize, 0);
        vector<u8> b2(kRandBufSize, 0);

        i32 r1 = sr1.read(&b1[0], kRandBufSize);
        i32 r2 = sr2.read(&b2[0], kRandBufSize);

        t.assert(r1 == kRandBufSize);
        t.assert(r2 == kRandBufSize);

        t.assert(bufs.find(b1) == bufs.end());
        bufs.insert(b1);
        t.assert(bufs.find(b1) != bufs.end());

        t.assert(bufs.find(b2) == bufs.end());
        bufs.insert(b2);
        t.assert(bufs.find(b2) != bufs.end());
    }
}


int main()
{
    tCrashReporter::init();

    tTest("tSecureRandom test", test, kNumTests);

    return 0;
}
