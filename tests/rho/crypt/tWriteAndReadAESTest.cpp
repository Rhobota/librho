#include <rho/crypt/tReadableAES.h>
#include <rho/crypt/tWritableAES.h>
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


static const int kNumKeys = 10;
static const int kNumItersPerKey = 100;
static const int kMaxMessageLength = 100000;


void print(const vector<u8>& v)
{
    for (size_t i = 0; i < v.size(); i++)
        cout << " " << (u32)v[i];
    cout << endl;
}


int numBytes(crypt::eKeyLengthAES keylen)
{
    switch (keylen)
    {
        case crypt::k128bit: return 16;
        case crypt::k192bit: return 24;
        case crypt::k256bit: return 32;
        default: return 0;
    }
}


void test(const tTest& t, u8* key, crypt::eKeyLengthAES keylen)
{
    tByteWritable bw;
    tByteReadable br;

    crypt::tWritableAES cw(&bw, key, keylen);
    crypt::tReadableAES cr(&br, key, keylen);

    //print(vector<u8>(key, key+numBytes(keylen)));

    for (int i = 0; i < kNumItersPerKey; i++)
    {
        // Gen a random message.
        int messagelen = rand() % kMaxMessageLength;
        vector<u8> pt1(messagelen);
        for (size_t j = 0; j < pt1.size(); j++)
            pt1[j] = rand() % 256;

        // Write the message to the AES writer (encryption).
        bw.reset();
        cw.writeAll(&pt1[0], pt1.size());
        cw.flush();

        // Read the message through the AES reader (decryption).
        vector<u8> ct = bw.getBuf();
        br.reset(ct);
        cr.reset();
        vector<u8> pt2(messagelen);
        cr.readAll(&pt2[0], pt2.size());

        // See if the original plain text matches the deciphered plain text.
        t.assert(pt1 == pt2);
    }
}


void test128(const tTest& t)
{
    u8 key128[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 };
    t.assert(sizeof(key128) == 128/8);

    for (size_t i = 0; i < sizeof(key128); i++)
        key128[i] = rand() % 256;

    test(t, key128, crypt::k128bit);
}


void test192(const tTest& t)
{
    u8 key192[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16,
                    17, 18, 19, 20, 21, 22, 23, 24 };
    t.assert(sizeof(key192) == 192/8);

    for (size_t i = 0; i < sizeof(key192); i++)
        key192[i] = rand() % 256;

    test(t, key192, crypt::k192bit);
}


void test256(const tTest& t)
{
    u8 key256[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16,
                    17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32 };
    t.assert(sizeof(key256) == 256/8);

    for (size_t i = 0; i < sizeof(key256); i++)
        key256[i] = rand() % 256;

    test(t, key256, crypt::k256bit);
}


int main()
{
    tCrashReporter::init();
    srand(time(0));

    tTest("tWriteAndReadAES 128bit test", test128, kNumKeys);
    tTest("tWriteAndReadAES 192bit test", test192, kNumKeys);
    tTest("tWriteAndReadAES 256bit test", test256, kNumKeys);

    return 0;
}
