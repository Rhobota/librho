#include <rho/iReadable.h>
#include <rho/iWritable.h>
#include <rho/iAsyncReadable.h>
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


static const int kNumTestIters = 100;
static const int kMaxMessageLength = 1000000;


void warpData(vector<u8>& data)
{
    switch (rand() % 3)
    {
        case 0:
            data.erase(data.begin() + (rand()%data.size()));
            break;
        case 1:
            data.insert(data.begin() + (rand()%data.size()), rand()%256);
            break;
        case 2:
            data[(rand()%data.size())] = rand()%256;
            break;
    }
}


void readInvalidStream(const tTest& t,
                       vector<u8>& ct,
                       const vector<u8>& pt1)
{
    warpData(ct);

    // Read the message through the zlib reader (inflations).
    {
        tByteReadable br;
        br.reset(ct);
        tZlibReadable zr(&br);
        try
        {
            vector<u8> pt2(kMaxMessageLength+50);
            i32 r = zr.readAll(&pt2[0], kMaxMessageLength+50);
            t.assert(r <= pt1.size());
            t.assert(r <= pt2.size());
            for (i32 i = 0; i < r; i++)
                t.assert(pt2[i] == pt1[i]);
        }
        catch (eRuntimeError& e)
        {
            // This is okay. Actually, this error is preferred given
            // the circumstances.
        }
    }

    // Read the message through the asynchronous zlib reader (inflations).
    {
        tByteAsyncReadable br;
        tZlibAsyncReadable zr(&br);
        try
        {
            zr.takeInput(&ct[0], (i32)ct.size());
            zr.endStream();
            const vector<u8>& pt2 = br.getBuf();
            t.assert(pt2.size() <= pt1.size());
            for (size_t i = 0; i < pt2.size(); i++)
                t.assert(pt2[i] == pt1[i]);
        }
        catch (eRuntimeError& e)
        {
            // This is okay. Actually, this error is preferred given
            // the circumstances.
        }
    }
}


void littleInvalidStreamTest(const tTest& t)
{
    tByteWritable bw;

    // Gen a random message.
    int messagelen = (rand() % 50) + 50;
    vector<u8> pt1(messagelen);
    for (size_t j = 0; j < pt1.size(); j++)
        pt1[j] = rand() % 256;

    // Write the message to the zlib writer (deflation).
    {
        tZlibWritable zw(&bw, (rand()%10));
        if (pt1.size() > 0)
        {
            i32 w = zw.writeAll(&pt1[0], pt1.size());
            t.assert(w >= 0 && ((size_t)w) == pt1.size());
        }
        t.assert(zw.flush());
    }
    vector<u8> ct = bw.getBuf();

    // Ensure the stream doesn't emit bad data.
    readInvalidStream(t, ct, pt1);
}


void largeInvalidStreamTest(const tTest& t)
{
    tByteWritable bw;

    // Gen a random message.
    int messagelen = (rand() % kMaxMessageLength) + 50;
    vector<u8> pt1(messagelen);
    for (size_t j = 0; j < pt1.size(); j++)
        pt1[j] = rand() % 256;

    // Write the message to the zlib writer (deflation).
    {
        tZlibWritable zw(&bw, (rand()%10));
        if (pt1.size() > 0)
        {
            i32 w = zw.writeAll(&pt1[0], pt1.size());
            t.assert(w >= 0 && ((size_t)w) == pt1.size());
        }
        t.assert(zw.flush());
    }
    vector<u8> ct = bw.getBuf();

    // Ensure the stream doesn't emit bad data.
    readInvalidStream(t, ct, pt1);
}


void littleTest(const tTest& t)
{
    tByteWritable bw;
    tByteReadable br;

    // Gen a random message.
    int messagelen = (rand() % 50);
    vector<u8> pt1(messagelen);
    for (size_t j = 0; j < pt1.size(); j++)
        pt1[j] = rand() % 256;

    // Write the message to the zlib writer (deflation).
    {
        tZlibWritable zw(&bw, (rand()%10));
        if (pt1.size() > 0)
        {
            i32 w = zw.writeAll(&pt1[0], pt1.size());
            t.assert(w >= 0 && ((size_t)w) == pt1.size());
        }
        t.assert(zw.flush());
    }
    vector<u8> ct = bw.getBuf();

    // Read the message through the zlib reader (inflations).
    br.reset(ct);
    tZlibReadable zr(&br);
    vector<u8> pt2(messagelen);
    i32 r = zr.readAll(&pt2[0], kMaxMessageLength);
    t.assert(r >= 0 && ((size_t)r) == pt2.size());

    // See if the original buf matches the resulting buf.
    t.assert(pt1 == pt2);
}


void largeTest1(const tTest& t)
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


void largeTest2(const tTest& t)
{
    tByteWritable bw;
    tByteReadable br;

    // Gen a random message.
    int messagelen = (rand() % kMaxMessageLength) + 1;
    vector<u8> pt1(messagelen);
    for (size_t j = 0; j < pt1.size(); j++)
        pt1[j] = rand() % 256;

    // Write the message to the zlib writer (deflation).
    {
        tZlibWritable zw(&bw, (rand()%10));
        i32 w = zw.writeAll(&pt1[0], pt1.size());
        t.assert(w >= 0 && ((size_t)w) == pt1.size());
        t.assert(zw.flush());
    }
    vector<u8> ct = bw.getBuf();

    // Read the message through the zlib reader (inflations).
    br.reset(ct);
    tZlibReadable zr(&br);
    vector<u8> pt2(messagelen);
    i32 r = zr.readAll(&pt2[0], 2*kMaxMessageLength);
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
    int center = 0, rem = 0;
    vector<u8> patt(len);
    for (int i = 0; i < len; i++)
    {
        if (rem == 0)
        {
            center = rand() % 256;
            rem = (rand() % 100) + 1;
        }
        int n = center + ((rand()%11)-5);
        if (n < 0) n = 0;
        if (n > 255) n = 255;
        patt[i] = n;
        rem--;
    }
    return patt;
}


void patter_littleTest(const tTest& t)
{
    tByteWritable bw;
    tByteReadable br;

    // Gen a random message.
    int messagelen = (rand() % 50);
    vector<u8> pt1 = getPatterData(messagelen);

    // Write the message to the zlib writer (deflation).
    {
        tZlibWritable zw(&bw, (rand()%10));
        if (pt1.size() > 0)
        {
            i32 w = zw.writeAll(&pt1[0], pt1.size());
            t.assert(w >= 0 && ((size_t)w) == pt1.size());
        }
        t.assert(zw.flush());
    }
    vector<u8> ct = bw.getBuf();

    // Read the message through the zlib reader (inflations).
    br.reset(ct);
    tZlibReadable zr(&br);
    vector<u8> pt2(messagelen);
    i32 r = zr.readAll(&pt2[0], kMaxMessageLength);
    t.assert(r >= 0 && ((size_t)r) == pt2.size());

    // See if the original buf matches the resulting buf.
    t.assert(pt1 == pt2);
}


void patter_largeTest1(const tTest& t)
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


void patter_largeTest2(const tTest& t)
{
    tByteWritable bw;
    tByteReadable br;

    // Gen a random message.
    int messagelen = (rand() % kMaxMessageLength) + 1;
    vector<u8> pt1 = getPatterData(messagelen);

    // Write the message to the zlib writer (deflation).
    {
        tZlibWritable zw(&bw, (rand()%10));
        i32 w = zw.writeAll(&pt1[0], pt1.size());
        t.assert(w >= 0 && ((size_t)w) == pt1.size());
        t.assert(zw.flush());
    }
    vector<u8> ct = bw.getBuf();

    // Read the message through the zlib reader (inflations).
    br.reset(ct);
    tZlibReadable zr(&br);
    vector<u8> pt2(messagelen);
    i32 r = zr.readAll(&pt2[0], 2*kMaxMessageLength);
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

    //tTest("zlib mem test", littleTest, 2000000000);

    tTest("zlib little invalid stream test", littleInvalidStreamTest, kNumTestIters);
    tTest("zlib large invalid stream test", largeInvalidStreamTest, kNumTestIters);

    tTest("zlib stream rand-data little test", littleTest, kNumTestIters);
    tTest("zlib stream rand-data large test (1)", largeTest1, kNumTestIters);
    tTest("zlib stream rand-data large test (2)", largeTest2, kNumTestIters);
    tTest("zlib stream rand-data large with random flush test (1)", largeWithRandomFlushesTest1, kNumTestIters);
    tTest("zlib stream rand-data large with random flush test (2)", largeWithRandomFlushesTest2, kNumTestIters);

    tTest("zlib stream pattern-data little test", patter_littleTest, kNumTestIters);
    tTest("zlib stream pattern-data large test (1)", patter_largeTest1, kNumTestIters);
    tTest("zlib stream pattern-data large test (2)", patter_largeTest2, kNumTestIters);
    tTest("zlib stream pattern-data large with random flush test (1)", patter_largeWithRandomFlushesTest1, kNumTestIters);
    tTest("zlib stream pattern-data large with random flush test (2)", patter_largeWithRandomFlushesTest2, kNumTestIters);

    return 0;
}
