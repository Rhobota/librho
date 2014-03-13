#include <rho/crypt/tMD5.h>
#include <rho/crypt/tSHA0.h>
#include <rho/crypt/tSHA1.h>
#include <rho/crypt/tSHA224.h>
#include <rho/crypt/tSHA256.h>
#include <rho/tCrashReporter.h>
#include <rho/tTest.h>

#include <iostream>
#include <string>
#include <vector>

using namespace rho;
using std::cout;
using std::endl;
using std::string;
using std::pair;
using std::make_pair;
using std::vector;


const int kNumIters = 1000;


string hash(crypt::iHasher& hasher, const string& input)
{
    size_t pos = 0;
    while (pos < input.size())
    {
        size_t dist = (rand() % (input.size()-pos)) + 1;
        hasher.write((u8*)&input[pos], dist);
        pos += dist;
    }
    return hasher.getHashString();
}


void test(crypt::iHasher& hasher, const pair<string,string>& tpair, const tTest& t)
{
    string res = hash(hasher, tpair.first);
    t.assert(res == tpair.second);
}


void testMD5(const tTest& t)
{
    vector< pair<string,string> > tests;

    tests.push_back(make_pair(string(""),
                              string("d41d8cd98f00b204e9800998ecf8427e")));

    tests.push_back(make_pair(string("a"),
                              string("0cc175b9c0f1b6a831c399e269772661")));

    tests.push_back(make_pair(string("abc"),
                              string("900150983cd24fb0d6963f7d28e17f72")));

    tests.push_back(make_pair(string("message digest"),
                              string("f96b697d7cb7938d525a2f31aaf161d0")));

    tests.push_back(make_pair(string("abcdefghijklmnopqrstuvwxyz"),
                              string("c3fcd3d76192e4007dfb496cca67e13b")));

    tests.push_back(make_pair(string("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789"),
                              string("d174ab98d277d9f5a5611c2c9f419d9f")));

    tests.push_back(make_pair(string("12345678901234567890123456789012345678901234567890123456789012345678901234567890"),
                              string("57edf4a22be3c955ac49da2e2107b67a")));

    for (size_t i = 0; i < tests.size(); i++)
    {
        crypt::tMD5 hasher;
        test(hasher, tests[i], t);
    }
}


void testSHA0(const tTest& t)
{
    vector< pair<string,string> > tests;

    tests.push_back(make_pair(string("abc"),
                              string("0164b8a914cd2a5e74c4f7ff082c4d97f1edf880")));

    for (size_t i = 0; i < tests.size(); i++)
    {
        crypt::tSHA0 hasher;
        test(hasher, tests[i], t);
    }
}


void testSHA1(const tTest& t)
{
    vector< pair<string,string> > tests;

    tests.push_back(make_pair(string(""),
                              string("da39a3ee5e6b4b0d3255bfef95601890afd80709")));

    tests.push_back(make_pair(string("a"),
                              string("86f7e437faa5a7fce15d1ddcb9eaeaea377667b8")));

    tests.push_back(make_pair(string("abc"),
                              string("a9993e364706816aba3e25717850c26c9cd0d89d")));

    tests.push_back(make_pair(string("message digest"),
                              string("c12252ceda8be8994d5fa0290a47231c1d16aae3")));

    tests.push_back(make_pair(string("abcdefghijklmnopqrstuvwxyz"),
                              string("32d10c7b8cf96570ca04ce37f2a19d84240d3a89")));

    tests.push_back(make_pair(string("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789"),
                              string("761c457bf73b14d27e9e9265c46f4b4dda11f940")));

    tests.push_back(make_pair(string("12345678901234567890123456789012345678901234567890123456789012345678901234567890"),
                              string("50abf5706a150990a08b2c5ea40fa0e585554732")));

    for (size_t i = 0; i < tests.size(); i++)
    {
        crypt::tSHA1 hasher;
        test(hasher, tests[i], t);
    }
}


void testSHA224(const tTest& t)
{
    vector< pair<string,string> > tests;

    tests.push_back(make_pair(string(""),
                              string("d14a028c2a3a2bc9476102bb288234c415a2b01f828ea62ac5b3e42f")));

    tests.push_back(make_pair(string("a"),
                              string("abd37534c7d9a2efb9465de931cd7055ffdb8879563ae98078d6d6d5")));

    tests.push_back(make_pair(string("abc"),
                              string("23097d223405d8228642a477bda255b32aadbce4bda0b3f7e36c9da7")));

    tests.push_back(make_pair(string("message digest"),
                              string("2cb21c83ae2f004de7e81c3c7019cbcb65b71ab656b22d6d0c39b8eb")));

    tests.push_back(make_pair(string("abcdefghijklmnopqrstuvwxyz"),
                              string("45a5f72c39c5cff2522eb3429799e49e5f44b356ef926bcf390dccc2")));

    tests.push_back(make_pair(string("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789"),
                              string("bff72b4fcb7d75e5632900ac5f90d219e05e97a7bde72e740db393d9")));

    tests.push_back(make_pair(string("12345678901234567890123456789012345678901234567890123456789012345678901234567890"),
                              string("b50aecbe4e9bb0b57bc5f3ae760a8e01db24f203fb3cdcd13148046e")));

    for (size_t i = 0; i < tests.size(); i++)
    {
        crypt::tSHA224 hasher;
        test(hasher, tests[i], t);
    }
}


void testSHA256(const tTest& t)
{
    vector< pair<string,string> > tests;

    tests.push_back(make_pair(string(""),
                              string("e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855")));

    tests.push_back(make_pair(string("a"),
                              string("ca978112ca1bbdcafac231b39a23dc4da786eff8147c4e72b9807785afee48bb")));

    tests.push_back(make_pair(string("abc"),
                              string("ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad")));

    tests.push_back(make_pair(string("message digest"),
                              string("f7846f55cf23e14eebeab5b4e1550cad5b509e3348fbc4efa3a1413d393cb650")));

    tests.push_back(make_pair(string("abcdefghijklmnopqrstuvwxyz"),
                              string("71c480df93d6ae2f1efad1447c66c9525e316218cf51fc8d9ed832f2daf18b73")));

    tests.push_back(make_pair(string("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789"),
                              string("db4bfcbd4da0cd85a60c3c37d3fbd8805c77f15fc6b1fdfe614ee0a7c8fdb4c0")));

    tests.push_back(make_pair(string("12345678901234567890123456789012345678901234567890123456789012345678901234567890"),
                              string("f371bc4a311f2b009eef952dd83ca80e2b60026c8e935592d0f9c308453c813e")));

    for (size_t i = 0; i < tests.size(); i++)
    {
        crypt::tSHA256 hasher;
        test(hasher, tests[i], t);
    }
}


int main()
{
    tCrashReporter::init();
    srand(time(0));

    tTest("tMD5 test", testMD5, kNumIters);
    tTest("tSHA0 test", testSHA0, kNumIters);
    tTest("tSHA1 test", testSHA1, kNumIters);
    tTest("tSHA224 test", testSHA224, kNumIters);
    tTest("tSHA256 test", testSHA256, kNumIters);

    return 0;
}
