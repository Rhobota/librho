#include <rho/crypt/tWritableAES.h>
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


void test(const tTest& t, tByteWritable& bw, crypt::tWritableAES& cw)
{
    vector<u8> pt;

    for (int i = 0; i < 1000; i++)         // <-- this loop should not exceed 512*AES_BLOCK_SIZE
    {                                      //     (because cw will auto-flush at that point)
        cw.flush();
        bw.reset();

        pt.push_back(rand() % 256);
        cw.writeAll(&pt[0], pt.size());
        cw.flush();
        vector<u8> ct = bw.getBuf();
        t.assert((ct.size() % AES_BLOCK_SIZE) == 0);

        u32 numBlocks = (pt.size() + 4) / AES_BLOCK_SIZE;
        if ((pt.size() + 4) % AES_BLOCK_SIZE)
            numBlocks++;
        t.assert(ct.size() == numBlocks*AES_BLOCK_SIZE);
    }
}


void test128(const tTest& t)
{
    u8 key128[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 };
    t.assert(sizeof(key128) == 128/8);

    tByteWritable bw;
    crypt::tWritableAES cw(&bw, key128, crypt::k128bit);
    test(t, bw, cw);
}


void test192(const tTest& t)
{
    u8 key192[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16,
                    17, 18, 19, 20, 21, 22, 23, 24 };
    t.assert(sizeof(key192) == 192/8);

    tByteWritable bw;
    crypt::tWritableAES cw(&bw, key192, crypt::k192bit);
    test(t, bw, cw);
}


void test256(const tTest& t)
{
    u8 key256[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16,
                    17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32 };
    t.assert(sizeof(key256) == 256/8);

    tByteWritable bw;
    crypt::tWritableAES cw(&bw, key256, crypt::k256bit);
    test(t, bw, cw);
}


int main()
{
    tCrashReporter::init();

    tTest("tWritableAES 128bit test", test128);
    tTest("tWritableAES 192bit test", test192);
    tTest("tWritableAES 256bit test", test256);

    return 0;
}
