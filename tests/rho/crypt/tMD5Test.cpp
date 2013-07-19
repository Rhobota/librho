#include <rho/crypt/tMD5.h>
#include <rho/tCrashReporter.h>
#include <rho/tTest.h>

#include <iostream>
#include <string>
#include <vector>

using namespace rho;
using std::cout;
using std::endl;
using std::vector;
using std::string;


string MD5(string stuff)
{
    crypt::tMD5 hasher;

    size_t pos = 0;
    while (pos < stuff.size())
    {
        size_t dist = (rand() % (stuff.size()-pos)) + 1;
        hasher.write((u8*)&stuff[pos], dist);
        pos += dist;
    }

    return hasher.getHashString();
}


void test(const tTest& t)
{
    t.assert(MD5("") == "d41d8cd98f00b204e9800998ecf8427e");
    t.assert(MD5("a") == "0cc175b9c0f1b6a831c399e269772661");
    t.assert(MD5("abc") == "900150983cd24fb0d6963f7d28e17f72");
    t.assert(MD5("message digest") == "f96b697d7cb7938d525a2f31aaf161d0");
    t.assert(MD5("abcdefghijklmnopqrstuvwxyz") == "c3fcd3d76192e4007dfb496cca67e13b");
    t.assert(MD5("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789") ==
                 "d174ab98d277d9f5a5611c2c9f419d9f");
    t.assert(MD5("12345678901234567890123456789012345678901234567890123"
                                     "456789012345678901234567890") ==
                 "57edf4a22be3c955ac49da2e2107b67a");
}


int main()
{
    tCrashReporter::init();
    srand(time(0));

    tTest("tMD5 test", test, 1000);

    return 0;
}
