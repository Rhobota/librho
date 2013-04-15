#include <rho/crypt/tReadableAES.h>
#include <rho/tCrashReporter.h>
#include <rho/tTest.h>

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
    crypt::tReadableAES cr(&br, key128, crypt::k128bit);
    test(t, br, cr);
}


void test192(const tTest& t)
{
    u8 key192[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16,
                    17, 18, 19, 20, 21, 22, 23, 24 };
    t.assert(sizeof(key192) == 192/8);

    tByteReadable br;
    crypt::tReadableAES cr(&br, key192, crypt::k192bit);
    test(t, br, cr);
}


void test256(const tTest& t)
{
    u8 key256[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16,
                    17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32 };
    t.assert(sizeof(key256) == 256/8);

    tByteReadable br;
    crypt::tReadableAES cr(&br, key256, crypt::k256bit);
    test(t, br, cr);
}


int main()
{
    tCrashReporter::init();

    tTest("tReadableAES 128bit test", test128);
    tTest("tReadableAES 192bit test", test192);
    tTest("tReadableAES 256bit test", test256);

    return 0;
}
