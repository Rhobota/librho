#include <rho/iReadable.h>
#include <rho/iWritable.h>
#include <rho/tCrashReporter.h>
#include <rho/tTest.h>

#include <cstdlib>
#include <ctime>
#include <iostream>
#include <vector>

using namespace rho;
using std::cout;
using std::endl;
using std::vector;


static const int kNumTestIters = 10;
static const int kMaxMessageLength = 1000000;


void simpleTest(const tTest& t)
{
    tByteWritable bw;
    tByteReadable br;

    tZlibWritable zw(&bw, (rand()%10));
    tZlibReadable zr(&br);

    // Gen a random message.
    int messagelen = (rand() % kMaxMessageLength) + 1;
    vector<u8> pt1(messagelen);
    for (size_t j = 0; j < pt1.size(); j++)
        pt1[j] = rand() % 256;

    // Write the message to the zlib writer (deflation).
    i32 w = zw.writeAll(&pt1[0], pt1.size());
    t.assert(w >= 0 && ((size_t)w) == pt1.size());
    t.assert(zw.flush());

    // Read the message through the zlib reader (inflations).
    vector<u8> ct = bw.getBuf();
    br.reset(ct);
    vector<u8> pt2(messagelen);
    i32 r = zr.readAll(&pt2[0], pt2.size());
    t.assert(r >= 0 && ((size_t)r) == pt2.size());

    // See if the original buf matches the resulting buf.
    t.assert(pt1 == pt2);
}


void randomFlushTest(const tTest& t)
{
    tByteWritable bw;
    tByteReadable br;

    tZlibWritable zw(&bw, (rand()%10));
    tZlibReadable zr(&br);

    // Gen a random message.
    int messagelen = (rand() % kMaxMessageLength) + 1;
    vector<u8> pt1(messagelen);
    for (size_t j = 0; j < pt1.size(); j++)
        pt1[j] = rand() % 256;

    // Write the message to the zlib writer (deflation).
    size_t w = 0;
    while (w < pt1.size())
    {
        if ((rand() % 10) == 0)
            t.assert(zw.flush());
        size_t len = (rand() % 20000) + 1;
        i32 w_here = zw.write(&pt1[w], std::min(len, pt1.size()-w));
        if (w_here <= 0)
            t.fail();
        w += w_here;
    }
    t.assert(w == pt1.size());
    t.assert(zw.flush());

    // Read the message through the zlib reader (inflations).
    vector<u8> ct = bw.getBuf();
    br.reset(ct);
    vector<u8> pt2(messagelen);
    i32 r = zr.readAll(&pt2[0], pt2.size());
    t.assert(r >= 0 && ((size_t)r) == pt2.size());

    // See if the original buf matches the resulting buf.
    t.assert(pt1 == pt2);
}


int main()
{
    tCrashReporter::init();
    srand(time(0));

    tTest("zlib stream simple test", simpleTest, kNumTestIters);
    tTest("zlib stream random flush test", randomFlushTest, kNumTestIters);

    return 0;
}
