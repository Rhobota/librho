#include <rho/crypt/tReadableAES.h>
#include <rho/crypt/tWritableAES.h>
#include <rho/tCrashReporter.h>
#include <rho/tTest.h>
#include <rho/sync/tTimer.h>

#include <iostream>
#include <vector>


using namespace rho;
using std::cout;
using std::endl;
using std::vector;


void print(const vector<u8>& v)
{
    for (size_t i = 0; i < v.size(); i++)
        cout << " " << (u32)v[i];
    cout << endl;
}


void test(const tTest& t, tByteReadable& br, crypt::tReadableAES& cr)
{
    // Not sure how to test this in isolation...
}


void test128(const tTest& t)
{
    u8 key128[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 };
    t.assert(sizeof(key128) == 128/8);

    tByteReadable br;
    crypt::tReadableAES cr(&br, crypt::kOpModeECB, key128, crypt::k128bit);
    test(t, br, cr);
}


void test192(const tTest& t)
{
    u8 key192[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16,
                    17, 18, 19, 20, 21, 22, 23, 24 };
    t.assert(sizeof(key192) == 192/8);

    tByteReadable br;
    crypt::tReadableAES cr(&br, crypt::kOpModeECB, key192, crypt::k192bit);
    test(t, br, cr);
}


void test256(const tTest& t)
{
    u8 key256[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16,
                    17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32 };
    t.assert(sizeof(key256) == 256/8);

    tByteReadable br;
    crypt::tReadableAES cr(&br, crypt::kOpModeECB, key256, crypt::k256bit);
    test(t, br, cr);
}


void testSpeed128(const tTest& t)
{
    u8 key128[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 };
    t.assert(sizeof(key128) == 128/8);

    tByteWritable bw;
    crypt::tWritableAES cw(&bw, crypt::kOpModeCBC, key128, crypt::k128bit);

    f64 start = sync::tTimer::usecTime();
    const i32 kBufSize = 4096;
    u8 buf[kBufSize];
    const i32 kTotalSize = 1000000000;
    for (i32 i = 0; i < kTotalSize; i += kBufSize)
    {
        i32 w = cw.writeAll(buf, kBufSize);
        t.assert(w == kBufSize);
    }
    cw.flush();
    f64 end = sync::tTimer::usecTime();
    std::cout << (end - start) / 1000000 << std::endl;

    tByteReadable br(bw.getBuf());
    crypt::tReadableAES cr(&br, crypt::kOpModeCBC, key128, crypt::k128bit);

    start = sync::tTimer::usecTime();
    while (true)
    {
        i32 r = cr.readAll(buf, kBufSize);
        if (r <= 0)
            break;
    }
    end = sync::tTimer::usecTime();
    std::cout << (end - start) / 1000000 << std::endl;
}


int main()
{
    tCrashReporter::init();

    tTest("tReadableAES 128bit test", test128);
    tTest("tReadableAES 192bit test", test192);
    tTest("tReadableAES 256bit test", test256);

    //tTest("tReadableAES 128bit speed test", testSpeed128);

    return 0;
}
