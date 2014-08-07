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


static const int kNumTestIters = 1000;
static const int kMaxMessageLength = 1000000;


void littleTest(const tTest& t)
{
    tByteWritable bw;
    tByteReadable br;

    tZlibWritable zw(&bw, (rand()%10));
    tZlibReadable zr(&br);

    // Gen a random message.
    int messagelen = (rand() % 50);
    vector<u8> pt1(messagelen);
    for (size_t j = 0; j < pt1.size(); j++)
        pt1[j] = rand() % 256;

    // Write the message to the zlib writer (deflation).
    if (pt1.size() > 0)
    {
        i32 w = zw.writeAll(&pt1[0], pt1.size());
        t.assert(w >= 0 && ((size_t)w) == pt1.size());
    }
    t.assert(zw.flush());

    // Read the message through the zlib reader (inflations).
    vector<u8> ct = bw.getBuf();
    br.reset(ct);
    vector<u8> pt2(messagelen);
    i32 r = zr.readAll(&pt2[0], 1000000);
    t.assert(r >= 0 && ((size_t)r) == pt2.size());

    // See if the original buf matches the resulting buf.
    t.assert(pt1 == pt2);
}


void largeTest(const tTest& t)
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


void largeWithRandomFlushesTest1(const tTest& t)
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
        if (w_here <= 0 || ((size_t)w_here) > std::min(len, pt1.size()-w))
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


void largeWithRandomFlushesTest2(const tTest& t)
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
        if (w_here <= 0 || ((size_t)w_here) > std::min(len, pt1.size()-w))
            t.fail();
        w += w_here;
    }
    t.assert(w == pt1.size());
    t.assert(zw.flush());

    // Read the message through the zlib reader (inflations).
    vector<u8> ct = bw.getBuf();
    br.reset(ct);
    vector<u8> pt2(messagelen);
    size_t r = 0;
    while (r < pt2.size())
    {
        size_t len = (rand() % 20000) + 1;
        i32 r_here = zr.read(&pt2[r], std::min(len, pt2.size()-r));
        if (r_here <= 0 || ((size_t)r_here) > std::min(len, pt2.size()-r))
            t.fail();
        r += r_here;
    }
    t.assert(r == pt2.size());

    // See if the original buf matches the resulting buf.
    t.assert(pt1 == pt2);
}


vector<u8> getPatterData(int len)
{
    return vector<u8>(len, 0);
}


void patter_littleTest(const tTest& t)
{
    tByteWritable bw;
    tByteReadable br;

    tZlibWritable zw(&bw, (rand()%10));
    tZlibReadable zr(&br);

    // Gen a random message.
    int messagelen = (rand() % 50);
    vector<u8> pt1 = getPatterData(messagelen);

    // Write the message to the zlib writer (deflation).
    if (pt1.size() > 0)
    {
        i32 w = zw.writeAll(&pt1[0], pt1.size());
        t.assert(w >= 0 && ((size_t)w) == pt1.size());
    }
    t.assert(zw.flush());

    // Read the message through the zlib reader (inflations).
    vector<u8> ct = bw.getBuf();
    br.reset(ct);
    vector<u8> pt2(messagelen);
    i32 r = zr.readAll(&pt2[0], 1000000);
    t.assert(r >= 0 && ((size_t)r) == pt2.size());

    // See if the original buf matches the resulting buf.
    t.assert(pt1 == pt2);
}


void patter_largeTest(const tTest& t)
{
    tByteWritable bw;
    tByteReadable br;

    tZlibWritable zw(&bw, (rand()%10));
    tZlibReadable zr(&br);

    // Gen a random message.
    int messagelen = (rand() % kMaxMessageLength) + 1;
    vector<u8> pt1 = getPatterData(messagelen);

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


void patter_largeWithRandomFlushesTest1(const tTest& t)
{
    tByteWritable bw;
    tByteReadable br;

    tZlibWritable zw(&bw, (rand()%10));
    tZlibReadable zr(&br);

    // Gen a random message.
    int messagelen = (rand() % kMaxMessageLength) + 1;
    vector<u8> pt1 = getPatterData(messagelen);

    // Write the message to the zlib writer (deflation).
    size_t w = 0;
    while (w < pt1.size())
    {
        if ((rand() % 10) == 0)
            t.assert(zw.flush());
        size_t len = (rand() % 20000) + 1;
        i32 w_here = zw.write(&pt1[w], std::min(len, pt1.size()-w));
        if (w_here <= 0 || ((size_t)w_here) > std::min(len, pt1.size()-w))
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


void patter_largeWithRandomFlushesTest2(const tTest& t)
{
    tByteWritable bw;
    tByteReadable br;

    tZlibWritable zw(&bw, (rand()%10));
    tZlibReadable zr(&br);

    // Gen a random message.
    int messagelen = (rand() % kMaxMessageLength) + 1;
    vector<u8> pt1 = getPatterData(messagelen);

    // Write the message to the zlib writer (deflation).
    size_t w = 0;
    while (w < pt1.size())
    {
        if ((rand() % 10) == 0)
            t.assert(zw.flush());
        size_t len = (rand() % 20000) + 1;
        i32 w_here = zw.write(&pt1[w], std::min(len, pt1.size()-w));
        if (w_here <= 0 || ((size_t)w_here) > std::min(len, pt1.size()-w))
            t.fail();
        w += w_here;
    }
    t.assert(w == pt1.size());
    t.assert(zw.flush());

    // Read the message through the zlib reader (inflations).
    vector<u8> ct = bw.getBuf();
    br.reset(ct);
    vector<u8> pt2(messagelen);
    size_t r = 0;
    while (r < pt2.size())
    {
        size_t len = (rand() % 20000) + 1;
        i32 r_here = zr.read(&pt2[r], std::min(len, pt2.size()-r));
        if (r_here <= 0 || ((size_t)r_here) > std::min(len, pt2.size()-r))
            t.fail();
        r += r_here;
    }
    t.assert(r == pt2.size());

    // See if the original buf matches the resulting buf.
    t.assert(pt1 == pt2);
}


int main()
{
    tCrashReporter::init();
    srand(time(0));

    tTest("zlib stream rand-data little test", littleTest, kNumTestIters);
    tTest("zlib stream rand-data large test", largeTest, kNumTestIters);
    tTest("zlib stream rand-data large with random flush test (1)", largeWithRandomFlushesTest1, kNumTestIters);
    tTest("zlib stream rand-data large with random flush test (2)", largeWithRandomFlushesTest2, kNumTestIters);

    tTest("zlib stream pattern-data little test", patter_littleTest, kNumTestIters);
    tTest("zlib stream pattern-data large test", patter_largeTest, kNumTestIters);
    tTest("zlib stream pattern-data large with random flush test (1)", patter_largeWithRandomFlushesTest1, kNumTestIters);
    tTest("zlib stream pattern-data large with random flush test (2)", patter_largeWithRandomFlushesTest2, kNumTestIters);

    return 0;
}
