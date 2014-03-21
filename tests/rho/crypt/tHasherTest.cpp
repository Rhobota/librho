#include <rho/crypt/hash_utils.h>
#include <rho/crypt/tMD5.h>
#include <rho/crypt/tSHA1.h>
#include <rho/crypt/tSHA224.h>
#include <rho/crypt/tSHA256.h>
#include <rho/crypt/tSHA384.h>
#include <rho/crypt/tSHA512.h>
#include <rho/crypt/tWhirlpool.h>
#include <rho/tCrashReporter.h>
#include <rho/tTest.h>

#include <iostream>
#include <string>
#include <vector>

using namespace rho;
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


void testSHA384(const tTest& t)
{
    vector< pair<string,string> > tests;

    tests.push_back(make_pair(string(""),
                              string("38b060a751ac96384cd9327eb1b1e36a21fdb71114be07434c0cc7bf63f6e1da274edebfe76f65fbd51ad2f14898b95b")));

    tests.push_back(make_pair(string("a"),
                              string("54a59b9f22b0b80880d8427e548b7c23abd873486e1f035dce9cd697e85175033caa88e6d57bc35efae0b5afd3145f31")));

    tests.push_back(make_pair(string("abc"),
                              string("cb00753f45a35e8bb5a03d699ac65007272c32ab0eded1631a8b605a43ff5bed8086072ba1e7cc2358baeca134c825a7")));

    tests.push_back(make_pair(string("message digest"),
                              string("473ed35167ec1f5d8e550368a3db39be54639f828868e9454c239fc8b52e3c61dbd0d8b4de1390c256dcbb5d5fd99cd5")));

    tests.push_back(make_pair(string("abcdefghijklmnopqrstuvwxyz"),
                              string("feb67349df3db6f5924815d6c3dc133f091809213731fe5c7b5f4999e463479ff2877f5f2936fa63bb43784b12f3ebb4")));

    tests.push_back(make_pair(string("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789"),
                              string("1761336e3f7cbfe51deb137f026f89e01a448e3b1fafa64039c1464ee8732f11a5341a6f41e0c202294736ed64db1a84")));

    tests.push_back(make_pair(string("12345678901234567890123456789012345678901234567890123456789012345678901234567890"),
                              string("b12932b0627d1c060942f5447764155655bd4da0c9afa6dd9b9ef53129af1b8fb0195996d2de9ca0df9d821ffee67026")));

    for (size_t i = 0; i < tests.size(); i++)
    {
        crypt::tSHA384 hasher;
        test(hasher, tests[i], t);
    }
}


void testSHA512(const tTest& t)
{
    vector< pair<string,string> > tests;

    tests.push_back(make_pair(string(""),
                              string("cf83e1357eefb8bdf1542850d66d8007d620e4050b5715dc83f4a921d36ce9ce47d0d13c5d85f2b0ff8318d2877eec2f63b931bd47417a81a538327af927da3e")));

    tests.push_back(make_pair(string("a"),
                              string("1f40fc92da241694750979ee6cf582f2d5d7d28e18335de05abc54d0560e0f5302860c652bf08d560252aa5e74210546f369fbbbce8c12cfc7957b2652fe9a75")));

    tests.push_back(make_pair(string("abc"),
                              string("ddaf35a193617abacc417349ae20413112e6fa4e89a97ea20a9eeee64b55d39a2192992a274fc1a836ba3c23a3feebbd454d4423643ce80e2a9ac94fa54ca49f")));

    tests.push_back(make_pair(string("message digest"),
                              string("107dbf389d9e9f71a3a95f6c055b9251bc5268c2be16d6c13492ea45b0199f3309e16455ab1e96118e8a905d5597b72038ddb372a89826046de66687bb420e7c")));

    tests.push_back(make_pair(string("abcdefghijklmnopqrstuvwxyz"),
                              string("4dbff86cc2ca1bae1e16468a05cb9881c97f1753bce3619034898faa1aabe429955a1bf8ec483d7421fe3c1646613a59ed5441fb0f321389f77f48a879c7b1f1")));

    tests.push_back(make_pair(string("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789"),
                              string("1e07be23c26a86ea37ea810c8ec7809352515a970e9253c26f536cfc7a9996c45c8370583e0a78fa4a90041d71a4ceab7423f19c71b9d5a3e01249f0bebd5894")));

    tests.push_back(make_pair(string("12345678901234567890123456789012345678901234567890123456789012345678901234567890"),
                              string("72ec1ef1124a45b047e8b7c75a932195135bb61de24ec0d1914042246e0aec3a2354e093d76f3048b456764346900cb130d2a4fd5dd16abb5e30bcb850dee843")));

    for (size_t i = 0; i < tests.size(); i++)
    {
        crypt::tSHA512 hasher;
        test(hasher, tests[i], t);
    }
}


void testWhirlpool(const tTest& t)
{
    vector< pair<string,string> > tests;

    tests.push_back(make_pair(string(""),
                              string("19fa61d75522a4669b44e39c1d2e1726c530232130d407f89afee0964997f7a73e83be698b288febcf88e3e03c4f0757ea8964e59b63d93708b138cc42a66eb3")));

    tests.push_back(make_pair(string("a"),
                              string("8aca2602792aec6f11a67206531fb7d7f0dff59413145e6973c45001d0087b42d11bc645413aeff63a42391a39145a591a92200d560195e53b478584fdae231a")));

    tests.push_back(make_pair(string("abc"),
                              string("4e2448a4c6f486bb16b6562c73b4020bf3043e3a731bce721ae1b303d97e6d4c7181eebdb6c57e277d0e34957114cbd6c797fc9d95d8b582d225292076d4eef5")));

    tests.push_back(make_pair(string("message digest"),
                              string("378c84a4126e2dc6e56dcc7458377aac838d00032230f53ce1f5700c0ffb4d3b8421557659ef55c106b4b52ac5a4aaa692ed920052838f3362e86dbd37a8903e")));

    tests.push_back(make_pair(string("abcdefghijklmnopqrstuvwxyz"),
                              string("f1d754662636ffe92c82ebb9212a484a8d38631ead4238f5442ee13b8054e41b08bf2a9251c30b6a0b8aae86177ab4a6f68f673e7207865d5d9819a3dba4eb3b")));

    tests.push_back(make_pair(string("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789"),
                              string("dc37e008cf9ee69bf11f00ed9aba26901dd7c28cdec066cc6af42e40f82f3a1e08eba26629129d8fb7cb57211b9281a65517cc879d7b962142c65f5a7af01467")));

    tests.push_back(make_pair(string("12345678901234567890123456789012345678901234567890123456789012345678901234567890"),
                              string("466ef18babb0154d25b9d38a6414f5c08784372bccb204d6549c4afadb6014294d5bd8df2a6c44e538cd047b2681a51a2c60481e88c5a20b2c2a80cf3a9a083b")));

    tests.push_back(make_pair(string("abcdbcdecdefdefgefghfghighijhijk"),
                              string("2a987ea40f917061f5d6f0a0e4644f488a7a5a52deee656207c562f988e95c6916bdc8031bc5be1b7b947639fe050b56939baaa0adff9ae6745b7b181c3be3fd")));

    for (size_t i = 0; i < tests.size(); i++)
    {
        crypt::tWhirlpool hasher;
        test(hasher, tests[i], t);
    }
}


void testHMAC_MD5(const tTest& t)
{
    vector< pair< pair<string,string>,string> > tests;

    tests.push_back(make_pair(make_pair(string(""),string("")),
                              string("74e6f7298a9c2d168935f58c001bad88")));

    tests.push_back(make_pair(make_pair(string("key"),string("The quick brown fox jumps over the lazy dog")),
                              string("80070713463e7749b90c2dc24911e275")));

    tests.push_back(make_pair(make_pair(string("asd;fkjasdfasdf9sadf9asdfkljd;fakjsdf;jasd;fkjasdfahsdfkals;jdf;aksdjf;aksdfj;aksjdfasdfereirerjasdfkjasdfasdfasdf"),string("asdjfasdfasdkfja;sdkfjasd9fasd908fas9d08fa0sdfasdfnmvhn,zxmvnskldfhsdofiudsf9uasdfadksfj;askdfj;asdkfhajksherfernamsdbnfasdfjughaesuiryhas;dkfhnas;dfiha;lskdhfa;hfa;sdkjhfa;sdkfha;sdkfhasd;fhernase.rhka.seklrbha.kdifha;lsdifyha;.erkih>Sdklea;lsdifhas d;fakj sd;fka sdfjhlaskjhfajsd;hfhasleufhajerbalsuer;lasker;aseirya;eriyha;sekhrfa;keiryha;so8eryawerkjane.resuihfD:SofiZeihf;awirha/.iwhef.zakdehfa;wiery;KHFA;SDKFH;h;kjadhf;adhksf")),
                              string("b2ee7e9351916e9c03fed49be6588a9c")));

    tests.push_back(make_pair(make_pair(string("\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b"),string("Hi There")),
                              string("9294727a3638bb1c13f48ef8158bfc9d")));

    tests.push_back(make_pair(make_pair(string("Jefe"),string("what do ya want for nothing?")),
                              string("750c783e6ab0b503eaa86e310a5db738")));

    tests.push_back(make_pair(make_pair(string("\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA"),string("\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD")),
                              string("56be34521d144c88dbb8c733f0e8b3f6")));

    tests.push_back(make_pair(make_pair(string("\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19"),string("\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd")),
                              string("697eaf0aca3a3aea3a75164746ffaa79")));

    tests.push_back(make_pair(make_pair(string("\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c"),string("Test With Truncation")),
                              string("56461ef2342edc00f9bab995690efd4c")));

    tests.push_back(make_pair(make_pair(string("\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa"),string("Test Using Larger Than Block-Size Key - Hash Key First")),
                              string("6b1ab7fe4bd7bf8f0b62e6ce61b9d0cd")));

    tests.push_back(make_pair(make_pair(string("\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa"),string("Test Using Larger Than Block-Size Key and Larger Than One Block-Size Data")),
                              string("6f630fad67cda0ee1fb1f562db3aa53e")));

    for (size_t i = 0; i < tests.size(); i++)
    {
        string key = tests[i].first.first;
        string msg = tests[i].first.second;
        string cor = tests[i].second;

        vector<u8> res = crypt::hmac_md5(vector<u8>(key.c_str(), key.c_str()+key.length()),
                                         vector<u8>(msg.c_str(), msg.c_str()+msg.length()));
        t.assert(crypt::hashToString(res) == cor);
    }
}


void testHMAC_SHA1(const tTest& t)
{
    vector< pair< pair<string,string>,string> > tests;

    tests.push_back(make_pair(make_pair(string(""),string("")),
                              string("fbdb1d1b18aa6c08324b7d64b71fb76370690e1d")));

    tests.push_back(make_pair(make_pair(string("key"),string("The quick brown fox jumps over the lazy dog")),
                              string("de7c9b85b8b78aa6bc8a7a36f70a90701c9db4d9")));

    tests.push_back(make_pair(make_pair(string("asd;fkjasdfasdf9sadf9asdfkljd;fakjsdf;jasd;fkjasdfahsdfkals;jdf;aksdjf;aksdfj;aksjdfasdfereirerjasdfkjasdfasdfasdf"),string("asdjfasdfasdkfja;sdkfjasd9fasd908fas9d08fa0sdfasdfnmvhn,zxmvnskldfhsdofiudsf9uasdfadksfj;askdfj;asdkfhajksherfernamsdbnfasdfjughaesuiryhas;dkfhnas;dfiha;lskdhfa;hfa;sdkjhfa;sdkfha;sdkfhasd;fhernase.rhka.seklrbha.kdifha;lsdifyha;.erkih>Sdklea;lsdifhas d;fakj sd;fka sdfjhlaskjhfajsd;hfhasleufhajerbalsuer;lasker;aseirya;eriyha;sekhrfa;keiryha;so8eryawerkjane.resuihfD:SofiZeihf;awirha/.iwhef.zakdehfa;wiery;KHFA;SDKFH;h;kjadhf;adhksf")),
                              string("87e96cd1d429c73c5a64ecd7226c7456b6cd6cb3")));

    tests.push_back(make_pair(make_pair(string("\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b"),string("Hi There")),
                              string("b617318655057264e28bc0b6fb378c8ef146be00")));

    tests.push_back(make_pair(make_pair(string("Jefe"),string("what do ya want for nothing?")),
                              string("effcdf6ae5eb2fa2d27416d5f184df9c259a7c79")));

    tests.push_back(make_pair(make_pair(string("\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA"),string("\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD")),
                              string("125d7342b9ac11cd91a39af48aa17b4f63f175d3")));

    tests.push_back(make_pair(make_pair(string("\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19"),string("\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd")),
                              string("4c9007f4026250c6bc8414f9bf50c86c2d7235da")));

    tests.push_back(make_pair(make_pair(string("\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c"),string("Test With Truncation")),
                              string("4c1a03424b55e07fe7f27be1d58bb9324a9a5a04")));

    tests.push_back(make_pair(make_pair(string("\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa"),string("Test Using Larger Than Block-Size Key - Hash Key First")),
                              string("aa4ae5e15272d00e95705637ce8a3b55ed402112")));

    tests.push_back(make_pair(make_pair(string("\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa"),string("Test Using Larger Than Block-Size Key and Larger Than One Block-Size Data")),
                              string("e8e99d0f45237d786d6bbaa7965c7808bbff1a91")));

    for (size_t i = 0; i < tests.size(); i++)
    {
        string key = tests[i].first.first;
        string msg = tests[i].first.second;
        string cor = tests[i].second;

        vector<u8> res = crypt::hmac_sha1(vector<u8>(key.c_str(), key.c_str()+key.length()),
                                          vector<u8>(msg.c_str(), msg.c_str()+msg.length()));
        t.assert(crypt::hashToString(res) == cor);
    }
}


void testHMAC_SHA224(const tTest& t)
{
    vector< pair< pair<string,string>,string> > tests;

    tests.push_back(make_pair(make_pair(string(""),string("")),
                              string("5ce14f72894662213e2748d2a6ba234b74263910cedde2f5a9271524")));

    tests.push_back(make_pair(make_pair(string("key"),string("The quick brown fox jumps over the lazy dog")),
                              string("88ff8b54675d39b8f72322e65ff945c52d96379988ada25639747e69")));

    tests.push_back(make_pair(make_pair(string("asd;fkjasdfasdf9sadf9asdfkljd;fakjsdf;jasd;fkjasdfahsdfkals;jdf;aksdjf;aksdfj;aksjdfasdfereirerjasdfkjasdfasdfasdf"),string("asdjfasdfasdkfja;sdkfjasd9fasd908fas9d08fa0sdfasdfnmvhn,zxmvnskldfhsdofiudsf9uasdfadksfj;askdfj;asdkfhajksherfernamsdbnfasdfjughaesuiryhas;dkfhnas;dfiha;lskdhfa;hfa;sdkjhfa;sdkfha;sdkfhasd;fhernase.rhka.seklrbha.kdifha;lsdifyha;.erkih>Sdklea;lsdifhas d;fakj sd;fka sdfjhlaskjhfajsd;hfhasleufhajerbalsuer;lasker;aseirya;eriyha;sekhrfa;keiryha;so8eryawerkjane.resuihfD:SofiZeihf;awirha/.iwhef.zakdehfa;wiery;KHFA;SDKFH;h;kjadhf;adhksf")),
                              string("417b31f48cfb6fc98cf53497e8eefb5d29a84238c91519a05cdb4c44")));

    tests.push_back(make_pair(make_pair(string("\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b"),string("Hi There")),
                              string("896fb1128abbdf196832107cd49df33f47b4b1169912ba4f53684b22")));

    tests.push_back(make_pair(make_pair(string("Jefe"),string("what do ya want for nothing?")),
                              string("a30e01098bc6dbbf45690f3a7e9e6d0f8bbea2a39e6148008fd05e44")));

    tests.push_back(make_pair(make_pair(string("\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA"),string("\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD")),
                              string("7fb3cb3588c6c1f6ffa9694d7d6ad2649365b0c1f65d69d1ec8333ea")));

    tests.push_back(make_pair(make_pair(string("\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19"),string("\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd")),
                              string("6c11506874013cac6a2abc1bb382627cec6a90d86efc012de7afec5a")));

    tests.push_back(make_pair(make_pair(string("\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa"),string("Test Using Larger Than Block-Size Key - Hash Key First")),
                              string("95e9a0db962095adaebe9b2d6f0dbce2d499f112f2d2b7273fa6870e")));

    tests.push_back(make_pair(make_pair(string("\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa"),string("This is a test using a larger than block-size key and a larger than block-size data. The key needs to be hashed before being used by the HMAC algorithm.")),
                              string("3a854166ac5d9f023f54d517d0b39dbd946770db9c2b95c9f6f565d1")));

    for (size_t i = 0; i < tests.size(); i++)
    {
        string key = tests[i].first.first;
        string msg = tests[i].first.second;
        string cor = tests[i].second;

        vector<u8> res = crypt::hmac_sha224(vector<u8>(key.c_str(), key.c_str()+key.length()),
                                            vector<u8>(msg.c_str(), msg.c_str()+msg.length()));
        t.assert(crypt::hashToString(res) == cor);
    }
}


void testHMAC_SHA256(const tTest& t)
{
    vector< pair< pair<string,string>,string> > tests;

    tests.push_back(make_pair(make_pair(string(""),string("")),
                              string("b613679a0814d9ec772f95d778c35fc5ff1697c493715653c6c712144292c5ad")));

    tests.push_back(make_pair(make_pair(string("key"),string("The quick brown fox jumps over the lazy dog")),
                              string("f7bc83f430538424b13298e6aa6fb143ef4d59a14946175997479dbc2d1a3cd8")));

    tests.push_back(make_pair(make_pair(string("asd;fkjasdfasdf9sadf9asdfkljd;fakjsdf;jasd;fkjasdfahsdfkals;jdf;aksdjf;aksdfj;aksjdfasdfereirerjasdfkjasdfasdfasdf"),string("asdjfasdfasdkfja;sdkfjasd9fasd908fas9d08fa0sdfasdfnmvhn,zxmvnskldfhsdofiudsf9uasdfadksfj;askdfj;asdkfhajksherfernamsdbnfasdfjughaesuiryhas;dkfhnas;dfiha;lskdhfa;hfa;sdkjhfa;sdkfha;sdkfhasd;fhernase.rhka.seklrbha.kdifha;lsdifyha;.erkih>Sdklea;lsdifhas d;fakj sd;fka sdfjhlaskjhfajsd;hfhasleufhajerbalsuer;lasker;aseirya;eriyha;sekhrfa;keiryha;so8eryawerkjane.resuihfD:SofiZeihf;awirha/.iwhef.zakdehfa;wiery;KHFA;SDKFH;h;kjadhf;adhksf")),
                              string("a002c286074712f51f88442234a043265bc80a3a58ea7bad9a2fb85747a1cf4a")));

    tests.push_back(make_pair(make_pair(string("\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b"),string("Hi There")),
                              string("b0344c61d8db38535ca8afceaf0bf12b881dc200c9833da726e9376c2e32cff7")));

    tests.push_back(make_pair(make_pair(string("Jefe"),string("what do ya want for nothing?")),
                              string("5bdcc146bf60754e6a042426089575c75a003f089d2739839dec58b964ec3843")));

    tests.push_back(make_pair(make_pair(string("\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA"),string("\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD")),
                              string("773ea91e36800e46854db8ebd09181a72959098b3ef8c122d9635514ced565fe")));

    tests.push_back(make_pair(make_pair(string("\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19"),string("\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd")),
                              string("82558a389a443c0ea4cc819899f2083a85f0faa3e578f8077a2e3ff46729665b")));

    tests.push_back(make_pair(make_pair(string("\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa"),string("Test Using Larger Than Block-Size Key - Hash Key First")),
                              string("60e431591ee0b67f0d8a26aacbf5b77f8e0bc6213728c5140546040f0ee37f54")));

    tests.push_back(make_pair(make_pair(string("\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa"),string("This is a test using a larger than block-size key and a larger than block-size data. The key needs to be hashed before being used by the HMAC algorithm.")),
                              string("9b09ffa71b942fcb27635fbcd5b0e944bfdc63644f0713938a7f51535c3a35e2")));

    for (size_t i = 0; i < tests.size(); i++)
    {
        string key = tests[i].first.first;
        string msg = tests[i].first.second;
        string cor = tests[i].second;

        vector<u8> res = crypt::hmac_sha256(vector<u8>(key.c_str(), key.c_str()+key.length()),
                                            vector<u8>(msg.c_str(), msg.c_str()+msg.length()));
        t.assert(crypt::hashToString(res) == cor);
    }
}


void testHMAC_SHA384(const tTest& t)
{
    vector< pair< pair<string,string>,string> > tests;

    tests.push_back(make_pair(make_pair(string(""),string("")),
                              string("6c1f2ee938fad2e24bd91298474382ca218c75db3d83e114b3d4367776d14d3551289e75e8209cd4b792302840234adc")));

    tests.push_back(make_pair(make_pair(string("key"),string("The quick brown fox jumps over the lazy dog")),
                              string("d7f4727e2c0b39ae0f1e40cc96f60242d5b7801841cea6fc592c5d3e1ae50700582a96cf35e1e554995fe4e03381c237")));

    tests.push_back(make_pair(make_pair(string("asd;fkjasdfasdf9sadf9asdfkljd;fakjsdf;jasd;fkjasdfahsdfkals;jdf;aksdjf;aksdfj;aksjdfasdfereirerjasdfkjasdfasdfasdf"),string("asdjfasdfasdkfja;sdkfjasd9fasd908fas9d08fa0sdfasdfnmvhn,zxmvnskldfhsdofiudsf9uasdfadksfj;askdfj;asdkfhajksherfernamsdbnfasdfjughaesuiryhas;dkfhnas;dfiha;lskdhfa;hfa;sdkjhfa;sdkfha;sdkfhasd;fhernase.rhka.seklrbha.kdifha;lsdifyha;.erkih>Sdklea;lsdifhas d;fakj sd;fka sdfjhlaskjhfajsd;hfhasleufhajerbalsuer;lasker;aseirya;eriyha;sekhrfa;keiryha;so8eryawerkjane.resuihfD:SofiZeihf;awirha/.iwhef.zakdehfa;wiery;KHFA;SDKFH;h;kjadhf;adhksf")),
                              string("f800902fa3f7e69e8f132069b5184ea34cfb0c8a72822edb7c08ceb05e8a33d393bb7d56846cb5b947cada326bbe6e66")));

    tests.push_back(make_pair(make_pair(string("\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b"),string("Hi There")),
                              string("afd03944d84895626b0825f4ab46907f15f9dadbe4101ec682aa034c7cebc59cfaea9ea9076ede7f4af152e8b2fa9cb6")));

    tests.push_back(make_pair(make_pair(string("Jefe"),string("what do ya want for nothing?")),
                              string("af45d2e376484031617f78d2b58a6b1b9c7ef464f5a01b47e42ec3736322445e8e2240ca5e69e2c78b3239ecfab21649")));

    tests.push_back(make_pair(make_pair(string("\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA"),string("\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD")),
                              string("88062608d3e6ad8a0aa2ace014c8a86f0aa635d947ac9febe83ef4e55966144b2a5ab39dc13814b94e3ab6e101a34f27")));

    tests.push_back(make_pair(make_pair(string("\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19"),string("\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd")),
                              string("3e8a69b7783c25851933ab6290af6ca77a9981480850009cc5577c6e1f573b4e6801dd23c4a7d679ccf8a386c674cffb")));

    tests.push_back(make_pair(make_pair(string("\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa"),string("Test Using Larger Than Block-Size Key - Hash Key First")),
                              string("4ece084485813e9088d2c63a041bc5b44f9ef1012a2b588f3cd11f05033ac4c60c2ef6ab4030fe8296248df163f44952")));

    tests.push_back(make_pair(make_pair(string("\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa"),string("This is a test using a larger than block-size key and a larger than block-size data. The key needs to be hashed before being used by the HMAC algorithm.")),
                              string("6617178e941f020d351e2f254e8fd32c602420feb0b8fb9adccebb82461e99c5a678cc31e799176d3860e6110c46523e")));

    for (size_t i = 0; i < tests.size(); i++)
    {
        string key = tests[i].first.first;
        string msg = tests[i].first.second;
        string cor = tests[i].second;

        vector<u8> res = crypt::hmac_sha384(vector<u8>(key.c_str(), key.c_str()+key.length()),
                                            vector<u8>(msg.c_str(), msg.c_str()+msg.length()));
        t.assert(crypt::hashToString(res) == cor);
    }
}


void testHMAC_SHA512(const tTest& t)
{
    vector< pair< pair<string,string>,string> > tests;

    tests.push_back(make_pair(make_pair(string(""),string("")),
                              string("b936cee86c9f87aa5d3c6f2e84cb5a4239a5fe50480a6ec66b70ab5b1f4ac6730c6c515421b327ec1d69402e53dfb49ad7381eb067b338fd7b0cb22247225d47")));

    tests.push_back(make_pair(make_pair(string("key"),string("The quick brown fox jumps over the lazy dog")),
                              string("b42af09057bac1e2d41708e48a902e09b5ff7f12ab428a4fe86653c73dd248fb82f948a549f7b791a5b41915ee4d1ec3935357e4e2317250d0372afa2ebeeb3a")));

    tests.push_back(make_pair(make_pair(string("asd;fkjasdfasdf9sadf9asdfkljd;fakjsdf;jasd;fkjasdfahsdfkals;jdf;aksdjf;aksdfj;aksjdfasdfereirerjasdfkjasdfasdfasdf"),string("asdjfasdfasdkfja;sdkfjasd9fasd908fas9d08fa0sdfasdfnmvhn,zxmvnskldfhsdofiudsf9uasdfadksfj;askdfj;asdkfhajksherfernamsdbnfasdfjughaesuiryhas;dkfhnas;dfiha;lskdhfa;hfa;sdkjhfa;sdkfha;sdkfhasd;fhernase.rhka.seklrbha.kdifha;lsdifyha;.erkih>Sdklea;lsdifhas d;fakj sd;fka sdfjhlaskjhfajsd;hfhasleufhajerbalsuer;lasker;aseirya;eriyha;sekhrfa;keiryha;so8eryawerkjane.resuihfD:SofiZeihf;awirha/.iwhef.zakdehfa;wiery;KHFA;SDKFH;h;kjadhf;adhksf")),
                              string("c689bde6ab4a3805ed3d90e77635b01b95b03697d3201116d86dca49fced98ae4ceee4936f4fe02c99a14e5d3e77e23d1b2e0749b2bb0aadf08ef5189a436aa6")));

    tests.push_back(make_pair(make_pair(string("\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b"),string("Hi There")),
                              string("87aa7cdea5ef619d4ff0b4241a1d6cb02379f4e2ce4ec2787ad0b30545e17cdedaa833b7d6b8a702038b274eaea3f4e4be9d914eeb61f1702e696c203a126854")));

    tests.push_back(make_pair(make_pair(string("Jefe"),string("what do ya want for nothing?")),
                              string("164b7a7bfcf819e2e395fbe73b56e0a387bd64222e831fd610270cd7ea2505549758bf75c05a994a6d034f65f8f0e6fdcaeab1a34d4a6b4b636e070a38bce737")));

    tests.push_back(make_pair(make_pair(string("\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA"),string("\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD")),
                              string("fa73b0089d56a284efb0f0756c890be9b1b5dbdd8ee81a3655f83e33b2279d39bf3e848279a722c806b485a47e67c807b946a337bee8942674278859e13292fb")));

    tests.push_back(make_pair(make_pair(string("\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19"),string("\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd")),
                              string("b0ba465637458c6990e5a8c5f61d4af7e576d97ff94b872de76f8050361ee3dba91ca5c11aa25eb4d679275cc5788063a5f19741120c4f2de2adebeb10a298dd")));

    tests.push_back(make_pair(make_pair(string("\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa"),string("Test Using Larger Than Block-Size Key - Hash Key First")),
                              string("80b24263c7c1a3ebb71493c1dd7be8b49b46d1f41b4aeec1121b013783f8f3526b56d037e05f2598bd0fd2215d6a1e5295e64f73f63f0aec8b915a985d786598")));

    tests.push_back(make_pair(make_pair(string("\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa"),string("This is a test using a larger than block-size key and a larger than block-size data. The key needs to be hashed before being used by the HMAC algorithm.")),
                              string("e37b6a775dc87dbaa4dfa9f96e5e3ffddebd71f8867289865df5a32d20cdc944b6022cac3c4982b10d5eeb55c3e4de15134676fb6de0446065c97440fa8c6a58")));

    for (size_t i = 0; i < tests.size(); i++)
    {
        string key = tests[i].first.first;
        string msg = tests[i].first.second;
        string cor = tests[i].second;

        vector<u8> res = crypt::hmac_sha512(vector<u8>(key.c_str(), key.c_str()+key.length()),
                                            vector<u8>(msg.c_str(), msg.c_str()+msg.length()));
        t.assert(crypt::hashToString(res) == cor);
    }
}


void testHMAC_WHIRLPOOL(const tTest& t)
{
    vector< pair< pair<string,string>,string> > tests;

    tests.push_back(make_pair(make_pair(string(""),string("")),
                              string("57d739903190550defa77309ff7b72406a927bbc54e8fcdc98e145fa4c36ce83a9cf1605ad01e0d1925f93ac1d12b985a26044e9fb1b9cce24301faa76eaab53")));

    tests.push_back(make_pair(make_pair(string("key"),string("The quick brown fox jumps over the lazy dog")),
                              string("7f7192e3a155cb6a8171584ba146882f26821658112dfd2601272db013517a31e573637d146584596f86a884eb0decc9514dde000ecf2476dc5d436a92197527")));

    tests.push_back(make_pair(make_pair(string("asd;fkjasdfasdf9sadf9asdfkljd;fakjsdf;jasd;fkjasdfahsdfkals;jdf;aksdjf;aksdfj;aksjdfasdfereirerjasdfkjasdfasdfasdf"),string("asdjfasdfasdkfja;sdkfjasd9fasd908fas9d08fa0sdfasdfnmvhn,zxmvnskldfhsdofiudsf9uasdfadksfj;askdfj;asdkfhajksherfernamsdbnfasdfjughaesuiryhas;dkfhnas;dfiha;lskdhfa;hfa;sdkjhfa;sdkfha;sdkfhasd;fhernase.rhka.seklrbha.kdifha;lsdifyha;.erkih>Sdklea;lsdifhas d;fakj sd;fka sdfjhlaskjhfajsd;hfhasleufhajerbalsuer;lasker;aseirya;eriyha;sekhrfa;keiryha;so8eryawerkjane.resuihfD:SofiZeihf;awirha/.iwhef.zakdehfa;wiery;KHFA;SDKFH;h;kjadhf;adhksf")),
                              string("2a15b48c162b1268698c453bb0e71113ef5b4e98ee53fb2dd3e64643df208f49a31c594c35271c981d52930d48761449345824f3457ccea3d22684884642b60d")));

    tests.push_back(make_pair(make_pair(string("\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b"),string("Hi There")),
                              string("27344136921cd495d3d36b3fc7094f9f3f26835e9b02bc5e215d75e082277e06723f0ad0dbb8008be3019f2a13a9103c99dd7f53eed96f92dc965eaef13c902c")));

    tests.push_back(make_pair(make_pair(string("Jefe"),string("what do ya want for nothing?")),
                              string("3d595ccd1d4f4cfd045af53ba7d5c8283fee6ded6eaf1269071b6b4ea64800056b5077c6a942cfa1221bd4e5aed791276e5dd46a407d2b8007163d3e7cd1de66")));

    tests.push_back(make_pair(make_pair(string("\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA"),string("\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD")),
                              string("844fb5124797a76cac689f5e359abb0a4a598ed594ba6163ca0ba245869760ce9ec2c507b29bf0aa21a9fc863bfeb0852bd43a0e5e290839acf7274e8ff20286")));

    tests.push_back(make_pair(make_pair(string("\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19"),string("\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd")),
                              string("35bc33e2ed71e1cb01c140ddd3291ae3f84e9f0dce18005a1123df199983a211fe744b244449a1c093b17584069359bc6a95352271d78e2ef7a6f21dc28ab3c1")));

    tests.push_back(make_pair(make_pair(string("\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c"),string("Test With Truncation")),
                              string("59f19e2aa74dd3893fb51338ec11d07b31abe491d0813e0ea01d2e2671c08511dac7795476cc37d41f04efce3c70cb0ec0e4f41f2c7dd4b5ff762e4a49f1826a")));

    tests.push_back(make_pair(make_pair(string("\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa"),string("Test Using Larger Than Block-Size Key - Hash Key First")),
                              string("dd90bd637cfcd27ca914c290f33402cf68576d6e70601af0295f6b9dafa9d988d8b8fb4fde8605ac544638158dba2baa90a2bf882546cd0b876d59ab3f18962e")));

    tests.push_back(make_pair(make_pair(string("\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa"),string("Test Using Larger Than Block-Size Key and Larger Than One Block-Size Data")),
                              string("faf529af18a0f569480fd001122bb446b32a3cf758c385cec4adad9c73c77327812e260b7082011a3eb1071e71a50e1c3d37a8963f8b6a64a31cc017db1d619c")));

    for (size_t i = 0; i < tests.size(); i++)
    {
        string key = tests[i].first.first;
        string msg = tests[i].first.second;
        string cor = tests[i].second;

        vector<u8> res = crypt::hmac_whirlpool(vector<u8>(key.c_str(), key.c_str()+key.length()),
                                               vector<u8>(msg.c_str(), msg.c_str()+msg.length()));
        t.assert(crypt::hashToString(res) == cor);
    }
}


void testPBKDF2_HMAC_SHA1(const tTest& t)
{
    {
        string pass = "password";
        string salt = "salt";
        u32 iters = 1;
        u32 dklen = 20;

        string res = crypt::hashToString(
                        crypt::pbkdf2(crypt::hmac_sha1,
                                      vector<u8>(pass.c_str(), pass.c_str()+pass.length()),
                                      vector<u8>(salt.c_str(), salt.c_str()+salt.length()),
                                      iters, dklen));

        t.assert(res == "0c60c80f961f0e71f3a9b524af6012062fe037a6");
    }
    {
        string pass = "password";
        string salt = "salt";
        u32 iters = 2;
        u32 dklen = 20;

        string res = crypt::hashToString(
                        crypt::pbkdf2(crypt::hmac_sha1,
                                      vector<u8>(pass.c_str(), pass.c_str()+pass.length()),
                                      vector<u8>(salt.c_str(), salt.c_str()+salt.length()),
                                      iters, dklen));

        t.assert(res == "ea6c014dc72d6f8ccd1ed92ace1d41f0d8de8957");
    }
    {
        string pass = "password";
        string salt = "salt";
        u32 iters = 4096;
        u32 dklen = 20;

        string res = crypt::hashToString(
                        crypt::pbkdf2(crypt::hmac_sha1,
                                      vector<u8>(pass.c_str(), pass.c_str()+pass.length()),
                                      vector<u8>(salt.c_str(), salt.c_str()+salt.length()),
                                      iters, dklen));

        t.assert(res == "4b007901b765489abead49d926f721d065a429c1");
    }
    {
        // Disabled because it takes too long to run.
//         string pass = "password";
//         string salt = "salt";
//         u32 iters = 16777216;
//         u32 dklen = 20;
//
//         string res = crypt::hashToString(
//                         crypt::pbkdf2(crypt::hmac_sha1,
//                                       vector<u8>(pass.c_str(), pass.c_str()+pass.length()),
//                                       vector<u8>(salt.c_str(), salt.c_str()+salt.length()),
//                                       iters, dklen));
//
//         t.assert(res == "eefe3d61cd4da4e4e9945b3d6ba2158c2634e984");
    }
    {
        string pass = "passwordPASSWORDpassword";
        string salt = "saltSALTsaltSALTsaltSALTsaltSALTsalt";
        u32 iters = 4096;
        u32 dklen = 25;

        string res = crypt::hashToString(
                        crypt::pbkdf2(crypt::hmac_sha1,
                                      vector<u8>(pass.c_str(), pass.c_str()+pass.length()),
                                      vector<u8>(salt.c_str(), salt.c_str()+salt.length()),
                                      iters, dklen));

        t.assert(res == "3d2eec4fe41c849b80c8d83662c0e44a8b291a964cf2f07038");
    }
    {
        string pass("pass\0word", 9);
        string salt("sa\0lt", 5);
        u32 iters = 4096;
        u32 dklen = 16;

        string res = crypt::hashToString(
                        crypt::pbkdf2(crypt::hmac_sha1,
                                      vector<u8>(pass.c_str(), pass.c_str()+pass.length()),
                                      vector<u8>(salt.c_str(), salt.c_str()+salt.length()),
                                      iters, dklen));

        t.assert(res == "56fa6aa75548099dcc37d7f03425e0c3");
    }
    {
        string pass = "a";
        string salt = "b";
        u32 iters = 1000;
        u32 dklen = 16;

        string res = crypt::hashToString(
                        crypt::pbkdf2(crypt::hmac_sha1,
                                      vector<u8>(pass.c_str(), pass.c_str()+pass.length()),
                                      vector<u8>(salt.c_str(), salt.c_str()+salt.length()),
                                      iters, dklen));

        t.assert(res == "8b0b1a28939a307631789dec57410a93");
    }
    {
        string pass = "a";
        string salt = "b";
        u32 iters = 10000;
        u32 dklen = 51;

        string res = crypt::hashToString(
                        crypt::pbkdf2(crypt::hmac_sha1,
                                      vector<u8>(pass.c_str(), pass.c_str()+pass.length()),
                                      vector<u8>(salt.c_str(), salt.c_str()+salt.length()),
                                      iters, dklen));

        t.assert(res == "324fe48b0921051ada4d754dcf2a793575224e571316f123d0010524257ac6d6d3e366e6ac7e19498193bd20fd7ea8b5a8198d");
    }
    {
        string pass = "aaksdjf;jf;akdasdfrurasdfer7u3535353okja;sdkfjasdfjaso89er35asdf";
        string salt = "bdfasdf89dasfas9dfasdf033jasdfkjasd;fkjasd;fkjadf#%#%33293423";
        u32 iters = 100;
        u32 dklen = 81;

        string res = crypt::hashToString(
                        crypt::pbkdf2(crypt::hmac_sha1,
                                      vector<u8>(pass.c_str(), pass.c_str()+pass.length()),
                                      vector<u8>(salt.c_str(), salt.c_str()+salt.length()),
                                      iters, dklen));

        t.assert(res == "cc8437fb66fbb6b6cd344530978e34ce98464d61601373817e69d41d9d82e9c7359b17ac092071e15f127195bd998e591cf2568fa28ccf284254add14037f1a6ecd32e88e0b4272cedca8bdfb375560b85");
    }
}


void testPBKDF2_HMAC_SHA512(const tTest& t)
{
    {
        string pass = "passDATAb00AB7YxDTT";
        string salt = "saltKEYbcTcXHCBxtjD";
        u32 iters = 1;
        u32 dklen = 64;

        string res = crypt::hashToString(
                crypt::pbkdf2(crypt::hmac_sha512,
                    vector<u8>(pass.c_str(), pass.c_str()+pass.length()),
                    vector<u8>(salt.c_str(), salt.c_str()+salt.length()),
                    iters, dklen));

        t.assert(res == "cbe6088ad4359af42e603c2a33760ef9d4017a7b2aad10af46f992c660a0b461ecb0dc2a79c2570941bea6a08d15d6887e79f32b132e1c134e9525eeddd744fa");
    }

    {
        string pass = "passDATAb00AB7YxDTT";
        string salt = "saltKEYbcTcXHCBxtjD";
        u32 iters = 100000;
        u32 dklen = 64;

        string res = crypt::hashToString(
                crypt::pbkdf2(crypt::hmac_sha512,
                    vector<u8>(pass.c_str(), pass.c_str()+pass.length()),
                    vector<u8>(salt.c_str(), salt.c_str()+salt.length()),
                    iters, dklen));

        t.assert(res == "accdcd8798ae5cd85804739015ef2a11e32591b7b7d16f76819b30b0d49d80e1abea6c9822b80a1fdfe421e26f5603eca8a47a64c9a004fb5af8229f762ff41f");
    }

    {
        string pass = "passDATAb00AB7YxDTTl";
        string salt = "saltKEYbcTcXHCBxtjD2";
        u32 iters = 1;
        u32 dklen = 64;

        string res = crypt::hashToString(
                crypt::pbkdf2(crypt::hmac_sha512,
                    vector<u8>(pass.c_str(), pass.c_str()+pass.length()),
                    vector<u8>(salt.c_str(), salt.c_str()+salt.length()),
                    iters, dklen));

        t.assert(res == "8e5074a9513c1f1512c9b1df1d8bffa9d8b4ef9105dfc16681222839560fb63264bed6aabf761f180e912a66e0b53d65ec88f6a1519e14804eba6dc9df137007");
    }

    {
        string pass = "passDATAb00AB7YxDTTl";
        string salt = "saltKEYbcTcXHCBxtjD2";
        u32 iters = 100000;
        u32 dklen = 64;

        string res = crypt::hashToString(
                crypt::pbkdf2(crypt::hmac_sha512,
                    vector<u8>(pass.c_str(), pass.c_str()+pass.length()),
                    vector<u8>(salt.c_str(), salt.c_str()+salt.length()),
                    iters, dklen));

        t.assert(res == "594256b0bd4d6c9f21a87f7ba5772a791a10e6110694f44365cd94670e57f1aecd797ef1d1001938719044c7f018026697845eb9ad97d97de36ab8786aab5096");
    }

    {
        string pass = "passDATAb00AB7YxDTTlR";
        string salt = "saltKEYbcTcXHCBxtjD2P";
        u32 iters = 1;
        u32 dklen = 64;

        string res = crypt::hashToString(
                crypt::pbkdf2(crypt::hmac_sha512,
                    vector<u8>(pass.c_str(), pass.c_str()+pass.length()),
                    vector<u8>(salt.c_str(), salt.c_str()+salt.length()),
                    iters, dklen));

        t.assert(res == "a6ac8c048a7dfd7b838da88f22c3fab5bff15d7cb8d83a62c6721a8faf6903eab6152cb7421026e36f2ffef661eb4384dc276495c71b5cab72e1c1a38712e56b");
    }

    {
        string pass = "passDATAb00AB7YxDTTlR";
        string salt = "saltKEYbcTcXHCBxtjD2P";
        u32 iters = 100000;
        u32 dklen = 64;

        string res = crypt::hashToString(
                crypt::pbkdf2(crypt::hmac_sha512,
                    vector<u8>(pass.c_str(), pass.c_str()+pass.length()),
                    vector<u8>(salt.c_str(), salt.c_str()+salt.length()),
                    iters, dklen));

        t.assert(res == "94ffc2b1a390b7b8a9e6a44922c330db2b193adcf082eecd06057197f35931a9d0ec0ee5c660744b50b61f23119b847e658d179a914807f4b8ab8eb9505af065");
    }

    {
        string pass = "passDATAb00AB7YxDTTlRH2dqxDx19GDxDV1zFMz7E6QVqKIzwOtMnlxQLttpE5";
        string salt = "saltKEYbcTcXHCBxtjD2PnBh44AIQ6XUOCESOhXpEp3HrcGMwbjzQKMSaf63IJe";
        u32 iters = 1;
        u32 dklen = 64;

        string res = crypt::hashToString(
                crypt::pbkdf2(crypt::hmac_sha512,
                    vector<u8>(pass.c_str(), pass.c_str()+pass.length()),
                    vector<u8>(salt.c_str(), salt.c_str()+salt.length()),
                    iters, dklen));

        t.assert(res == "e2ccc7827f1dd7c33041a98906a8fd7bae1920a55fcb8f831683f14f1c3979351cb868717e5ab342d9a11acf0b12d3283931d609b06602da33f8377d1f1f9902");
    }

    {
        string pass = "passDATAb00AB7YxDTTlRH2dqxDx19GDxDV1zFMz7E6QVqKIzwOtMnlxQLttpE5";
        string salt = "saltKEYbcTcXHCBxtjD2PnBh44AIQ6XUOCESOhXpEp3HrcGMwbjzQKMSaf63IJe";
        u32 iters = 100000;
        u32 dklen = 64;

        string res = crypt::hashToString(
                crypt::pbkdf2(crypt::hmac_sha512,
                    vector<u8>(pass.c_str(), pass.c_str()+pass.length()),
                    vector<u8>(salt.c_str(), salt.c_str()+salt.length()),
                    iters, dklen));

        t.assert(res == "07447401c85766e4aed583de2e6bf5a675eabe4f3618281c95616f4fc1fdfe6ecbc1c3982789d4fd941d6584ef534a78bd37ae02555d9455e8f089fdb4dfb6bb");
    }

    {
        string pass = "passDATAb00AB7YxDTTlRH2dqxDx19GDxDV1zFMz7E6QVqKIzwOtMnlxQLttpE57";
        string salt = "saltKEYbcTcXHCBxtjD2PnBh44AIQ6XUOCESOhXpEp3HrcGMwbjzQKMSaf63IJem";
        u32 iters = 1;
        u32 dklen = 64;

        string res = crypt::hashToString(
                crypt::pbkdf2(crypt::hmac_sha512,
                    vector<u8>(pass.c_str(), pass.c_str()+pass.length()),
                    vector<u8>(salt.c_str(), salt.c_str()+salt.length()),
                    iters, dklen));

        t.assert(res == "b029a551117ff36977f283f579dc7065b352266ea243bdd3f920f24d4d141ed8b6e02d96e2d3bdfb76f8d77ba8f4bb548996ad85bb6f11d01a015ce518f9a717");
    }

    {
        string pass = "passDATAb00AB7YxDTTlRH2dqxDx19GDxDV1zFMz7E6QVqKIzwOtMnlxQLttpE57";
        string salt = "saltKEYbcTcXHCBxtjD2PnBh44AIQ6XUOCESOhXpEp3HrcGMwbjzQKMSaf63IJem";
        u32 iters = 100000;
        u32 dklen = 64;

        string res = crypt::hashToString(
                crypt::pbkdf2(crypt::hmac_sha512,
                    vector<u8>(pass.c_str(), pass.c_str()+pass.length()),
                    vector<u8>(salt.c_str(), salt.c_str()+salt.length()),
                    iters, dklen));

        t.assert(res == "31f5cc83ed0e948c05a15735d818703aaa7bff3f09f5169caf5dba6602a05a4d5cff5553d42e82e40516d6dc157b8daeae61d3fea456d964cb2f7f9a63bbbdb5");
    }

    {
        string pass = "passDATAb00AB7YxDTTlRH2dqxDx19GDxDV1zFMz7E6QVqKIzwOtMnlxQLttpE57U";
        string salt = "saltKEYbcTcXHCBxtjD2PnBh44AIQ6XUOCESOhXpEp3HrcGMwbjzQKMSaf63IJemk";
        u32 iters = 1;
        u32 dklen = 64;

        string res = crypt::hashToString(
                crypt::pbkdf2(crypt::hmac_sha512,
                    vector<u8>(pass.c_str(), pass.c_str()+pass.length()),
                    vector<u8>(salt.c_str(), salt.c_str()+salt.length()),
                    iters, dklen));

        t.assert(res == "28b8a9f644d6800612197bb74df460272e2276de8cc07ac4897ac24dbc6eb77499fcaf97415244d9a29da83fc347d09a5dbcfd6bd63ff6e410803dca8a900ab6");
    }

    {
        string pass = "passDATAb00AB7YxDTTlRH2dqxDx19GDxDV1zFMz7E6QVqKIzwOtMnlxQLttpE57U";
        string salt = "saltKEYbcTcXHCBxtjD2PnBh44AIQ6XUOCESOhXpEp3HrcGMwbjzQKMSaf63IJemk";
        u32 iters = 100000;
        u32 dklen = 64;

        string res = crypt::hashToString(
                crypt::pbkdf2(crypt::hmac_sha512,
                    vector<u8>(pass.c_str(), pass.c_str()+pass.length()),
                    vector<u8>(salt.c_str(), salt.c_str()+salt.length()),
                    iters, dklen));

        t.assert(res == "056bc9072a356b7d4da60dd66f5968c2caa375c0220eda6b47ef8e8d105ed68b44185fe9003fbba49e2c84240c9e8fd3f5b2f4f6512fd936450253db37d10028");
    }

    {
        string pass = "passDATAb00AB7YxDTTlRH2dqxDx19GDxDV1zFMz7E6QVqKIzwOtMnlxQLttpE57Un4u12D2YD7oOPpiEvCDYvntXEe4NNPLCnGGeJArbYDEu6xDoCfWH6kbuV6awi0";
        string salt = "saltKEYbcTcXHCBxtjD2PnBh44AIQ6XUOCESOhXpEp3HrcGMwbjzQKMSaf63IJemkURWoqHusIeVB8Il91NjiCGQacPUu9qTFaShLbKG0Yj4RCMV56WPj7E14EMpbxy";
        u32 iters = 1;
        u32 dklen = 64;

        string res = crypt::hashToString(
                crypt::pbkdf2(crypt::hmac_sha512,
                    vector<u8>(pass.c_str(), pass.c_str()+pass.length()),
                    vector<u8>(salt.c_str(), salt.c_str()+salt.length()),
                    iters, dklen));

        t.assert(res == "16226c85e4f8d604573008bfe61c10b6947b53990450612dd4a3077f7dee2116229e68efd1df6d73bd3c6d07567790eea1e8b2ae9a1b046be593847d9441a1b7");
    }

    {
        string pass = "passDATAb00AB7YxDTTlRH2dqxDx19GDxDV1zFMz7E6QVqKIzwOtMnlxQLttpE57Un4u12D2YD7oOPpiEvCDYvntXEe4NNPLCnGGeJArbYDEu6xDoCfWH6kbuV6awi0";
        string salt = "saltKEYbcTcXHCBxtjD2PnBh44AIQ6XUOCESOhXpEp3HrcGMwbjzQKMSaf63IJemkURWoqHusIeVB8Il91NjiCGQacPUu9qTFaShLbKG0Yj4RCMV56WPj7E14EMpbxy";
        u32 iters = 100000;
        u32 dklen = 64;

        string res = crypt::hashToString(
                crypt::pbkdf2(crypt::hmac_sha512,
                    vector<u8>(pass.c_str(), pass.c_str()+pass.length()),
                    vector<u8>(salt.c_str(), salt.c_str()+salt.length()),
                    iters, dklen));

        t.assert(res == "70cf39f14c4caf3c81fa288fb46c1db52d19f72722f7bc84f040676d3371c89c11c50f69bcfbc3acb0ab9e92e4ef622727a916219554b2fa121bedda97ff3332");
    }

    {
        string pass = "passDATAb00AB7YxDTTlRH2dqxDx19GDxDV1zFMz7E6QVqKIzwOtMnlxQLttpE57Un4u12D2YD7oOPpiEvCDYvntXEe4NNPLCnGGeJArbYDEu6xDoCfWH6kbuV6awi04";
        string salt = "saltKEYbcTcXHCBxtjD2PnBh44AIQ6XUOCESOhXpEp3HrcGMwbjzQKMSaf63IJemkURWoqHusIeVB8Il91NjiCGQacPUu9qTFaShLbKG0Yj4RCMV56WPj7E14EMpbxy6";
        u32 iters = 1;
        u32 dklen = 64;

        string res = crypt::hashToString(
                crypt::pbkdf2(crypt::hmac_sha512,
                    vector<u8>(pass.c_str(), pass.c_str()+pass.length()),
                    vector<u8>(salt.c_str(), salt.c_str()+salt.length()),
                    iters, dklen));

        t.assert(res == "880c58c316d3a5b9f05977ab9c60c10abeebfad5ce89cae62905c1c4f80a0a098d82f95321a6220f8aeccfb45ce6107140899e8d655306ae6396553e2851376c");
    }

    {
        string pass = "passDATAb00AB7YxDTTlRH2dqxDx19GDxDV1zFMz7E6QVqKIzwOtMnlxQLttpE57Un4u12D2YD7oOPpiEvCDYvntXEe4NNPLCnGGeJArbYDEu6xDoCfWH6kbuV6awi04";
        string salt = "saltKEYbcTcXHCBxtjD2PnBh44AIQ6XUOCESOhXpEp3HrcGMwbjzQKMSaf63IJemkURWoqHusIeVB8Il91NjiCGQacPUu9qTFaShLbKG0Yj4RCMV56WPj7E14EMpbxy6";
        u32 iters = 100000;
        u32 dklen = 64;

        string res = crypt::hashToString(
                crypt::pbkdf2(crypt::hmac_sha512,
                    vector<u8>(pass.c_str(), pass.c_str()+pass.length()),
                    vector<u8>(salt.c_str(), salt.c_str()+salt.length()),
                    iters, dklen));

        t.assert(res == "2668b71b3ca56136b5e87f30e098f6b4371cb5ed95537c7a073dac30a2d5be52756adf5bb2f4320cb11c4e16b24965a9c790def0cbc62906920b4f2eb84d1d4a");
    }

    {
        string pass = "passDATAb00AB7YxDTTlRH2dqxDx19GDxDV1zFMz7E6QVqKIzwOtMnlxQLttpE57Un4u12D2YD7oOPpiEvCDYvntXEe4NNPLCnGGeJArbYDEu6xDoCfWH6kbuV6awi04U";
        string salt = "saltKEYbcTcXHCBxtjD2PnBh44AIQ6XUOCESOhXpEp3HrcGMwbjzQKMSaf63IJemkURWoqHusIeVB8Il91NjiCGQacPUu9qTFaShLbKG0Yj4RCMV56WPj7E14EMpbxy6P";
        u32 iters = 1;
        u32 dklen = 64;

        string res = crypt::hashToString(
                crypt::pbkdf2(crypt::hmac_sha512,
                    vector<u8>(pass.c_str(), pass.c_str()+pass.length()),
                    vector<u8>(salt.c_str(), salt.c_str()+salt.length()),
                    iters, dklen));

        t.assert(res == "93b9ba8283cc17d50ef3b44820828a258a996de258225d24fb59990a6d0de82dfb3fe2ac201952100e4cc8f06d883a9131419c0f6f5a6ecb8ec821545f14adf1");
    }

    {
        string pass = "passDATAb00AB7YxDTTlRH2dqxDx19GDxDV1zFMz7E6QVqKIzwOtMnlxQLttpE57Un4u12D2YD7oOPpiEvCDYvntXEe4NNPLCnGGeJArbYDEu6xDoCfWH6kbuV6awi04U";
        string salt = "saltKEYbcTcXHCBxtjD2PnBh44AIQ6XUOCESOhXpEp3HrcGMwbjzQKMSaf63IJemkURWoqHusIeVB8Il91NjiCGQacPUu9qTFaShLbKG0Yj4RCMV56WPj7E14EMpbxy6P";
        u32 iters = 100000;
        u32 dklen = 64;

        string res = crypt::hashToString(
                crypt::pbkdf2(crypt::hmac_sha512,
                    vector<u8>(pass.c_str(), pass.c_str()+pass.length()),
                    vector<u8>(salt.c_str(), salt.c_str()+salt.length()),
                    iters, dklen));

        t.assert(res == "2575b485afdf37c260b8f3386d33a60ed929993c9d48ac516ec66b87e06be54ade7e7c8cb3417c81603b080a8eefc56072811129737ced96236b9364e22ce3a5");
    }

    {
        string pass = "passDATAb00AB7YxDTTlRH2dqxDx19GDxDV1zFMz7E6QVqKIzwOtMnlxQLttpE57Un4u12D2YD7oOPpiEvCDYvntXEe4NNPLCnGGeJArbYDEu6xDoCfWH6kbuV6awi04Uz3ebEAhzZ4ve1A2wg5CnLXdZC5Y7gwfVgbEgZSTmoYQSzC5OW4dfrjqiwApTACO6xoOL1AjWj6X6f6qFfF8TVmOzU9RhOd1N4QtzWI4fP6FYttNz5FuLdtYVXWVXH2Tf7I9fieMeWCHTMkM4VcmQyQHpbcP8MEb5f1g6Ckg5xk3HQr3wMBvQcOHpCPy1K8HCM7a5wkPDhgVA0BVmwNpsRIbDQZRtHK6dT6bGyalp6gbFZBuBHwD86gTzkrFY7HkOVrgc0gJcGJZe65Ce8v4Jn5OzkuVsiU8efm2Pw2RnbpWSAr7SkVdCwXK2XSJDQ5fZ4HBEz9VTFYrG23ELuLjvx5njOLNgDAJuf5JB2tn4nMjjcnl1e8qcYVwZqFzEv2zhLyDWMkV4tzl4asLnvyAxTBkxPRZj2pRABWwb3kEofpsHYxMTAn38YSpZreoXipZWBnu6HDURaruXaIPYFPYHl9Ls9wsuD7rzaGfbOyfVgLIGK5rODphwRA7lm88bGKY8b7tWOtepyEvaLxMI7GZF5ScwpZTYeEDNUKPzvM2Im9zehIaznpguNdNXNMLWnwPu4H6zEvajkw3G3ucSiXKmh6XNe3hkdSANm3vnxzRXm4fcuzAx68IElXE2bkGFElluDLo6EsUDWZ4JIWBVaDwYdJx8uCXbQdoifzCs5kuuClaDaDqIhb5hJ2WR8mxiueFsS0aDGdIYmye5svmNmzQxFmdOkHoF7CfwuU1yy4uEEt9vPSP2wFp1dyaMvJW68vtB4kddLmI6gIgVVcT6ZX1Qm6WsusPrdisPLB2ScodXojCbL3DLj6PKG8QDVMWTrL1TpafT2wslRledWIhsTlv2mI3C066WMcTSwKLXdEDhVvFJ6ShiLKSN7gnRrlE0BnAw";
        string salt = "saltKEYbcTcXHCBxtjD2PnBh44AIQ6XUOCESOhXpEp3HrcGMwbjzQKMSaf63IJemkURWoqHusIeVB8Il91NjiCGQacPUu9qTFaShLbKG0Yj4RCMV56WPj7E14EMpbxy6PlBdILBOkKUB6TGTPJXh1tpdOHTG6KuIvcbQp9qWjaf1uxAKgiTtYRIHhxjJI2viVa6fDZ67QOouOaf2RXQhpsWaTtAVnff6PIFcvJhdPDFGV5nvmZWoCZQodj6yXRDHPw9PyF0iLYm9uFtEunlAAxGB5qqea4X5tZvB1OfLVwymY3a3JPjdxTdvHxCHbqqE0zip61JNqdmeWxGtlRBC6CGoCiHO4XxHCntQBRJDcG0zW7joTdgtTBarsQQhlLXBGMNBSNmmTbDf3hFtawUBCJH18IAiRMwyeQJbJ2bERsY3MVRPuYCf4Au7gN72iGh1lRktSQtEFye7pO46kMXRrEjHQWXInMzzy7X2StXUzHVTFF2VdOoKn0WUqFNvB6PF7qIsOlYKj57bi1Psa34s85WxMSbTkhrd7VHdHZkTVaWdraohXYOePdeEvIwObCGEXkETUzqM5P2yzoBOJSdjpIYaa8zzdLD3yrb1TwCZuJVxsrq0XXY6vErU4QntsW0972XmGNyumFNJiPm4ONKh1RLvS1kddY3nm8276S4TUuZfrRQO8QxZRNuSaZI8JRZp5VojB5DktuMxAQkqoPjQ5Vtb6oXeOyY591CB1MEW1fLTCs0NrL321SaNRMqza1ETogAxpEiYwZ6pIgnMmSqNMRdZnCqA4gMWw1lIVATWK83OCeicNRUNOdfzS7A8vbLcmvKPtpOFvhNzwrrUdkvuKvaYJviQgeR7snGetO9JLCwIlHIj52gMCNU18d32SJl7Xomtl3wIe02SMvq1i1BcaX7lXioqWGmgVqBWU3fsUuGwHi6RUKCCQdEOBfNo2WdpFaCflcgnn0O6jVHCqkv8cQk81AqS00rAmHGCNTwyA6Tq5TXoLlDnC8gAQjDUsZp0z";
        u32 iters = 1;
        u32 dklen = 64;

        string res = crypt::hashToString(
                crypt::pbkdf2(crypt::hmac_sha512,
                    vector<u8>(pass.c_str(), pass.c_str()+pass.length()),
                    vector<u8>(salt.c_str(), salt.c_str()+salt.length()),
                    iters, dklen));

        t.assert(res == "384bcd6914407e40c295d1037cf4f990e8f0e720af43cb706683177016d36d1a14b3a7cf22b5df8d5d7d44d69610b64251ade2e7ab54a3813a89935592e391bf");
    }

    {
        string pass = "passDATAb00AB7YxDTTlRH2dqxDx19GDxDV1zFMz7E6QVqKIzwOtMnlxQLttpE57Un4u12D2YD7oOPpiEvCDYvntXEe4NNPLCnGGeJArbYDEu6xDoCfWH6kbuV6awi04Uz3ebEAhzZ4ve1A2wg5CnLXdZC5Y7gwfVgbEgZSTmoYQSzC5OW4dfrjqiwApTACO6xoOL1AjWj6X6f6qFfF8TVmOzU9RhOd1N4QtzWI4fP6FYttNz5FuLdtYVXWVXH2Tf7I9fieMeWCHTMkM4VcmQyQHpbcP8MEb5f1g6Ckg5xk3HQr3wMBvQcOHpCPy1K8HCM7a5wkPDhgVA0BVmwNpsRIbDQZRtHK6dT6bGyalp6gbFZBuBHwD86gTzkrFY7HkOVrgc0gJcGJZe65Ce8v4Jn5OzkuVsiU8efm2Pw2RnbpWSAr7SkVdCwXK2XSJDQ5fZ4HBEz9VTFYrG23ELuLjvx5njOLNgDAJuf5JB2tn4nMjjcnl1e8qcYVwZqFzEv2zhLyDWMkV4tzl4asLnvyAxTBkxPRZj2pRABWwb3kEofpsHYxMTAn38YSpZreoXipZWBnu6HDURaruXaIPYFPYHl9Ls9wsuD7rzaGfbOyfVgLIGK5rODphwRA7lm88bGKY8b7tWOtepyEvaLxMI7GZF5ScwpZTYeEDNUKPzvM2Im9zehIaznpguNdNXNMLWnwPu4H6zEvajkw3G3ucSiXKmh6XNe3hkdSANm3vnxzRXm4fcuzAx68IElXE2bkGFElluDLo6EsUDWZ4JIWBVaDwYdJx8uCXbQdoifzCs5kuuClaDaDqIhb5hJ2WR8mxiueFsS0aDGdIYmye5svmNmzQxFmdOkHoF7CfwuU1yy4uEEt9vPSP2wFp1dyaMvJW68vtB4kddLmI6gIgVVcT6ZX1Qm6WsusPrdisPLB2ScodXojCbL3DLj6PKG8QDVMWTrL1TpafT2wslRledWIhsTlv2mI3C066WMcTSwKLXdEDhVvFJ6ShiLKSN7gnRrlE0BnAw";
        string salt = "saltKEYbcTcXHCBxtjD2PnBh44AIQ6XUOCESOhXpEp3HrcGMwbjzQKMSaf63IJemkURWoqHusIeVB8Il91NjiCGQacPUu9qTFaShLbKG0Yj4RCMV56WPj7E14EMpbxy6PlBdILBOkKUB6TGTPJXh1tpdOHTG6KuIvcbQp9qWjaf1uxAKgiTtYRIHhxjJI2viVa6fDZ67QOouOaf2RXQhpsWaTtAVnff6PIFcvJhdPDFGV5nvmZWoCZQodj6yXRDHPw9PyF0iLYm9uFtEunlAAxGB5qqea4X5tZvB1OfLVwymY3a3JPjdxTdvHxCHbqqE0zip61JNqdmeWxGtlRBC6CGoCiHO4XxHCntQBRJDcG0zW7joTdgtTBarsQQhlLXBGMNBSNmmTbDf3hFtawUBCJH18IAiRMwyeQJbJ2bERsY3MVRPuYCf4Au7gN72iGh1lRktSQtEFye7pO46kMXRrEjHQWXInMzzy7X2StXUzHVTFF2VdOoKn0WUqFNvB6PF7qIsOlYKj57bi1Psa34s85WxMSbTkhrd7VHdHZkTVaWdraohXYOePdeEvIwObCGEXkETUzqM5P2yzoBOJSdjpIYaa8zzdLD3yrb1TwCZuJVxsrq0XXY6vErU4QntsW0972XmGNyumFNJiPm4ONKh1RLvS1kddY3nm8276S4TUuZfrRQO8QxZRNuSaZI8JRZp5VojB5DktuMxAQkqoPjQ5Vtb6oXeOyY591CB1MEW1fLTCs0NrL321SaNRMqza1ETogAxpEiYwZ6pIgnMmSqNMRdZnCqA4gMWw1lIVATWK83OCeicNRUNOdfzS7A8vbLcmvKPtpOFvhNzwrrUdkvuKvaYJviQgeR7snGetO9JLCwIlHIj52gMCNU18d32SJl7Xomtl3wIe02SMvq1i1BcaX7lXioqWGmgVqBWU3fsUuGwHi6RUKCCQdEOBfNo2WdpFaCflcgnn0O6jVHCqkv8cQk81AqS00rAmHGCNTwyA6Tq5TXoLlDnC8gAQjDUsZp0z";
        u32 iters = 100000;
        u32 dklen = 64;

        string res = crypt::hashToString(
                crypt::pbkdf2(crypt::hmac_sha512,
                    vector<u8>(pass.c_str(), pass.c_str()+pass.length()),
                    vector<u8>(salt.c_str(), salt.c_str()+salt.length()),
                    iters, dklen));

        t.assert(res == "b8674f6c0cc9f8cf1f1874534fd5af01fc1504d76c2bc2aa0a75fe4dd5dfd1daf60ea7c85f122bceeb8772659d601231607726998eac3f6aab72eff7ba349f7f");
    }

    {
        string pass = "passDATAb00AB7YxDTT";
        string salt = "saltKEYbcTcXHCBxtjD";
        u32 iters = 1;
        u32 dklen = 63;

        string res = crypt::hashToString(
                crypt::pbkdf2(crypt::hmac_sha512,
                    vector<u8>(pass.c_str(), pass.c_str()+pass.length()),
                    vector<u8>(salt.c_str(), salt.c_str()+salt.length()),
                    iters, dklen));

        t.assert(res == "cbe6088ad4359af42e603c2a33760ef9d4017a7b2aad10af46f992c660a0b461ecb0dc2a79c2570941bea6a08d15d6887e79f32b132e1c134e9525eeddd744");
    }

    {
        string pass = "passDATAb00AB7YxDTT";
        string salt = "saltKEYbcTcXHCBxtjD";
        u32 iters = 100000;
        u32 dklen = 63;

        string res = crypt::hashToString(
                crypt::pbkdf2(crypt::hmac_sha512,
                    vector<u8>(pass.c_str(), pass.c_str()+pass.length()),
                    vector<u8>(salt.c_str(), salt.c_str()+salt.length()),
                    iters, dklen));

        t.assert(res == "accdcd8798ae5cd85804739015ef2a11e32591b7b7d16f76819b30b0d49d80e1abea6c9822b80a1fdfe421e26f5603eca8a47a64c9a004fb5af8229f762ff4");
    }

    {
        string pass = "passDATAb00AB7YxDTTl";
        string salt = "saltKEYbcTcXHCBxtjD2";
        u32 iters = 1;
        u32 dklen = 63;

        string res = crypt::hashToString(
                crypt::pbkdf2(crypt::hmac_sha512,
                    vector<u8>(pass.c_str(), pass.c_str()+pass.length()),
                    vector<u8>(salt.c_str(), salt.c_str()+salt.length()),
                    iters, dklen));

        t.assert(res == "8e5074a9513c1f1512c9b1df1d8bffa9d8b4ef9105dfc16681222839560fb63264bed6aabf761f180e912a66e0b53d65ec88f6a1519e14804eba6dc9df1370");
    }

    {
        string pass = "passDATAb00AB7YxDTTl";
        string salt = "saltKEYbcTcXHCBxtjD2";
        u32 iters = 100000;
        u32 dklen = 63;

        string res = crypt::hashToString(
                crypt::pbkdf2(crypt::hmac_sha512,
                    vector<u8>(pass.c_str(), pass.c_str()+pass.length()),
                    vector<u8>(salt.c_str(), salt.c_str()+salt.length()),
                    iters, dklen));

        t.assert(res == "594256b0bd4d6c9f21a87f7ba5772a791a10e6110694f44365cd94670e57f1aecd797ef1d1001938719044c7f018026697845eb9ad97d97de36ab8786aab50");
    }

    {
        string pass = "passDATAb00AB7YxDTTlR";
        string salt = "saltKEYbcTcXHCBxtjD2P";
        u32 iters = 1;
        u32 dklen = 63;

        string res = crypt::hashToString(
                crypt::pbkdf2(crypt::hmac_sha512,
                    vector<u8>(pass.c_str(), pass.c_str()+pass.length()),
                    vector<u8>(salt.c_str(), salt.c_str()+salt.length()),
                    iters, dklen));

        t.assert(res == "a6ac8c048a7dfd7b838da88f22c3fab5bff15d7cb8d83a62c6721a8faf6903eab6152cb7421026e36f2ffef661eb4384dc276495c71b5cab72e1c1a38712e5");
    }

    {
        string pass = "passDATAb00AB7YxDTTlR";
        string salt = "saltKEYbcTcXHCBxtjD2P";
        u32 iters = 100000;
        u32 dklen = 63;

        string res = crypt::hashToString(
                crypt::pbkdf2(crypt::hmac_sha512,
                    vector<u8>(pass.c_str(), pass.c_str()+pass.length()),
                    vector<u8>(salt.c_str(), salt.c_str()+salt.length()),
                    iters, dklen));

        t.assert(res == "94ffc2b1a390b7b8a9e6a44922c330db2b193adcf082eecd06057197f35931a9d0ec0ee5c660744b50b61f23119b847e658d179a914807f4b8ab8eb9505af0");
    }

    {
        string pass = "passDATAb00AB7YxDTTlRH2dqxDx19GDxDV1zFMz7E6QVqKIzwOtMnlxQLttpE5";
        string salt = "saltKEYbcTcXHCBxtjD2PnBh44AIQ6XUOCESOhXpEp3HrcGMwbjzQKMSaf63IJe";
        u32 iters = 1;
        u32 dklen = 63;

        string res = crypt::hashToString(
                crypt::pbkdf2(crypt::hmac_sha512,
                    vector<u8>(pass.c_str(), pass.c_str()+pass.length()),
                    vector<u8>(salt.c_str(), salt.c_str()+salt.length()),
                    iters, dklen));

        t.assert(res == "e2ccc7827f1dd7c33041a98906a8fd7bae1920a55fcb8f831683f14f1c3979351cb868717e5ab342d9a11acf0b12d3283931d609b06602da33f8377d1f1f99");
    }

    {
        string pass = "passDATAb00AB7YxDTTlRH2dqxDx19GDxDV1zFMz7E6QVqKIzwOtMnlxQLttpE5";
        string salt = "saltKEYbcTcXHCBxtjD2PnBh44AIQ6XUOCESOhXpEp3HrcGMwbjzQKMSaf63IJe";
        u32 iters = 100000;
        u32 dklen = 63;

        string res = crypt::hashToString(
                crypt::pbkdf2(crypt::hmac_sha512,
                    vector<u8>(pass.c_str(), pass.c_str()+pass.length()),
                    vector<u8>(salt.c_str(), salt.c_str()+salt.length()),
                    iters, dklen));

        t.assert(res == "07447401c85766e4aed583de2e6bf5a675eabe4f3618281c95616f4fc1fdfe6ecbc1c3982789d4fd941d6584ef534a78bd37ae02555d9455e8f089fdb4dfb6");
    }

    {
        string pass = "passDATAb00AB7YxDTTlRH2dqxDx19GDxDV1zFMz7E6QVqKIzwOtMnlxQLttpE57";
        string salt = "saltKEYbcTcXHCBxtjD2PnBh44AIQ6XUOCESOhXpEp3HrcGMwbjzQKMSaf63IJem";
        u32 iters = 1;
        u32 dklen = 63;

        string res = crypt::hashToString(
                crypt::pbkdf2(crypt::hmac_sha512,
                    vector<u8>(pass.c_str(), pass.c_str()+pass.length()),
                    vector<u8>(salt.c_str(), salt.c_str()+salt.length()),
                    iters, dklen));

        t.assert(res == "b029a551117ff36977f283f579dc7065b352266ea243bdd3f920f24d4d141ed8b6e02d96e2d3bdfb76f8d77ba8f4bb548996ad85bb6f11d01a015ce518f9a7");
    }

    {
        string pass = "passDATAb00AB7YxDTTlRH2dqxDx19GDxDV1zFMz7E6QVqKIzwOtMnlxQLttpE57";
        string salt = "saltKEYbcTcXHCBxtjD2PnBh44AIQ6XUOCESOhXpEp3HrcGMwbjzQKMSaf63IJem";
        u32 iters = 100000;
        u32 dklen = 63;

        string res = crypt::hashToString(
                crypt::pbkdf2(crypt::hmac_sha512,
                    vector<u8>(pass.c_str(), pass.c_str()+pass.length()),
                    vector<u8>(salt.c_str(), salt.c_str()+salt.length()),
                    iters, dklen));

        t.assert(res == "31f5cc83ed0e948c05a15735d818703aaa7bff3f09f5169caf5dba6602a05a4d5cff5553d42e82e40516d6dc157b8daeae61d3fea456d964cb2f7f9a63bbbd");
    }

    {
        string pass = "passDATAb00AB7YxDTTlRH2dqxDx19GDxDV1zFMz7E6QVqKIzwOtMnlxQLttpE57U";
        string salt = "saltKEYbcTcXHCBxtjD2PnBh44AIQ6XUOCESOhXpEp3HrcGMwbjzQKMSaf63IJemk";
        u32 iters = 1;
        u32 dklen = 63;

        string res = crypt::hashToString(
                crypt::pbkdf2(crypt::hmac_sha512,
                    vector<u8>(pass.c_str(), pass.c_str()+pass.length()),
                    vector<u8>(salt.c_str(), salt.c_str()+salt.length()),
                    iters, dklen));

        t.assert(res == "28b8a9f644d6800612197bb74df460272e2276de8cc07ac4897ac24dbc6eb77499fcaf97415244d9a29da83fc347d09a5dbcfd6bd63ff6e410803dca8a900a");
    }

    {
        string pass = "passDATAb00AB7YxDTTlRH2dqxDx19GDxDV1zFMz7E6QVqKIzwOtMnlxQLttpE57U";
        string salt = "saltKEYbcTcXHCBxtjD2PnBh44AIQ6XUOCESOhXpEp3HrcGMwbjzQKMSaf63IJemk";
        u32 iters = 100000;
        u32 dklen = 63;

        string res = crypt::hashToString(
                crypt::pbkdf2(crypt::hmac_sha512,
                    vector<u8>(pass.c_str(), pass.c_str()+pass.length()),
                    vector<u8>(salt.c_str(), salt.c_str()+salt.length()),
                    iters, dklen));

        t.assert(res == "056bc9072a356b7d4da60dd66f5968c2caa375c0220eda6b47ef8e8d105ed68b44185fe9003fbba49e2c84240c9e8fd3f5b2f4f6512fd936450253db37d100");
    }

    {
        string pass = "passDATAb00AB7YxDTTlRH2dqxDx19GDxDV1zFMz7E6QVqKIzwOtMnlxQLttpE57Un4u12D2YD7oOPpiEvCDYvntXEe4NNPLCnGGeJArbYDEu6xDoCfWH6kbuV6awi0";
        string salt = "saltKEYbcTcXHCBxtjD2PnBh44AIQ6XUOCESOhXpEp3HrcGMwbjzQKMSaf63IJemkURWoqHusIeVB8Il91NjiCGQacPUu9qTFaShLbKG0Yj4RCMV56WPj7E14EMpbxy";
        u32 iters = 1;
        u32 dklen = 63;

        string res = crypt::hashToString(
                crypt::pbkdf2(crypt::hmac_sha512,
                    vector<u8>(pass.c_str(), pass.c_str()+pass.length()),
                    vector<u8>(salt.c_str(), salt.c_str()+salt.length()),
                    iters, dklen));

        t.assert(res == "16226c85e4f8d604573008bfe61c10b6947b53990450612dd4a3077f7dee2116229e68efd1df6d73bd3c6d07567790eea1e8b2ae9a1b046be593847d9441a1");
    }

    {
        string pass = "passDATAb00AB7YxDTTlRH2dqxDx19GDxDV1zFMz7E6QVqKIzwOtMnlxQLttpE57Un4u12D2YD7oOPpiEvCDYvntXEe4NNPLCnGGeJArbYDEu6xDoCfWH6kbuV6awi0";
        string salt = "saltKEYbcTcXHCBxtjD2PnBh44AIQ6XUOCESOhXpEp3HrcGMwbjzQKMSaf63IJemkURWoqHusIeVB8Il91NjiCGQacPUu9qTFaShLbKG0Yj4RCMV56WPj7E14EMpbxy";
        u32 iters = 100000;
        u32 dklen = 63;

        string res = crypt::hashToString(
                crypt::pbkdf2(crypt::hmac_sha512,
                    vector<u8>(pass.c_str(), pass.c_str()+pass.length()),
                    vector<u8>(salt.c_str(), salt.c_str()+salt.length()),
                    iters, dklen));

        t.assert(res == "70cf39f14c4caf3c81fa288fb46c1db52d19f72722f7bc84f040676d3371c89c11c50f69bcfbc3acb0ab9e92e4ef622727a916219554b2fa121bedda97ff33");
    }

    {
        string pass = "passDATAb00AB7YxDTTlRH2dqxDx19GDxDV1zFMz7E6QVqKIzwOtMnlxQLttpE57Un4u12D2YD7oOPpiEvCDYvntXEe4NNPLCnGGeJArbYDEu6xDoCfWH6kbuV6awi04";
        string salt = "saltKEYbcTcXHCBxtjD2PnBh44AIQ6XUOCESOhXpEp3HrcGMwbjzQKMSaf63IJemkURWoqHusIeVB8Il91NjiCGQacPUu9qTFaShLbKG0Yj4RCMV56WPj7E14EMpbxy6";
        u32 iters = 1;
        u32 dklen = 63;

        string res = crypt::hashToString(
                crypt::pbkdf2(crypt::hmac_sha512,
                    vector<u8>(pass.c_str(), pass.c_str()+pass.length()),
                    vector<u8>(salt.c_str(), salt.c_str()+salt.length()),
                    iters, dklen));

        t.assert(res == "880c58c316d3a5b9f05977ab9c60c10abeebfad5ce89cae62905c1c4f80a0a098d82f95321a6220f8aeccfb45ce6107140899e8d655306ae6396553e285137");
    }

    {
        string pass = "passDATAb00AB7YxDTTlRH2dqxDx19GDxDV1zFMz7E6QVqKIzwOtMnlxQLttpE57Un4u12D2YD7oOPpiEvCDYvntXEe4NNPLCnGGeJArbYDEu6xDoCfWH6kbuV6awi04";
        string salt = "saltKEYbcTcXHCBxtjD2PnBh44AIQ6XUOCESOhXpEp3HrcGMwbjzQKMSaf63IJemkURWoqHusIeVB8Il91NjiCGQacPUu9qTFaShLbKG0Yj4RCMV56WPj7E14EMpbxy6";
        u32 iters = 100000;
        u32 dklen = 63;

        string res = crypt::hashToString(
                crypt::pbkdf2(crypt::hmac_sha512,
                    vector<u8>(pass.c_str(), pass.c_str()+pass.length()),
                    vector<u8>(salt.c_str(), salt.c_str()+salt.length()),
                    iters, dklen));

        t.assert(res == "2668b71b3ca56136b5e87f30e098f6b4371cb5ed95537c7a073dac30a2d5be52756adf5bb2f4320cb11c4e16b24965a9c790def0cbc62906920b4f2eb84d1d");
    }

    {
        string pass = "passDATAb00AB7YxDTTlRH2dqxDx19GDxDV1zFMz7E6QVqKIzwOtMnlxQLttpE57Un4u12D2YD7oOPpiEvCDYvntXEe4NNPLCnGGeJArbYDEu6xDoCfWH6kbuV6awi04U";
        string salt = "saltKEYbcTcXHCBxtjD2PnBh44AIQ6XUOCESOhXpEp3HrcGMwbjzQKMSaf63IJemkURWoqHusIeVB8Il91NjiCGQacPUu9qTFaShLbKG0Yj4RCMV56WPj7E14EMpbxy6P";
        u32 iters = 1;
        u32 dklen = 63;

        string res = crypt::hashToString(
                crypt::pbkdf2(crypt::hmac_sha512,
                    vector<u8>(pass.c_str(), pass.c_str()+pass.length()),
                    vector<u8>(salt.c_str(), salt.c_str()+salt.length()),
                    iters, dklen));

        t.assert(res == "93b9ba8283cc17d50ef3b44820828a258a996de258225d24fb59990a6d0de82dfb3fe2ac201952100e4cc8f06d883a9131419c0f6f5a6ecb8ec821545f14ad");
    }

    {
        string pass = "passDATAb00AB7YxDTTlRH2dqxDx19GDxDV1zFMz7E6QVqKIzwOtMnlxQLttpE57Un4u12D2YD7oOPpiEvCDYvntXEe4NNPLCnGGeJArbYDEu6xDoCfWH6kbuV6awi04U";
        string salt = "saltKEYbcTcXHCBxtjD2PnBh44AIQ6XUOCESOhXpEp3HrcGMwbjzQKMSaf63IJemkURWoqHusIeVB8Il91NjiCGQacPUu9qTFaShLbKG0Yj4RCMV56WPj7E14EMpbxy6P";
        u32 iters = 100000;
        u32 dklen = 63;

        string res = crypt::hashToString(
                crypt::pbkdf2(crypt::hmac_sha512,
                    vector<u8>(pass.c_str(), pass.c_str()+pass.length()),
                    vector<u8>(salt.c_str(), salt.c_str()+salt.length()),
                    iters, dklen));

        t.assert(res == "2575b485afdf37c260b8f3386d33a60ed929993c9d48ac516ec66b87e06be54ade7e7c8cb3417c81603b080a8eefc56072811129737ced96236b9364e22ce3");
    }

    {
        string pass = "passDATAb00AB7YxDTTlRH2dqxDx19GDxDV1zFMz7E6QVqKIzwOtMnlxQLttpE57Un4u12D2YD7oOPpiEvCDYvntXEe4NNPLCnGGeJArbYDEu6xDoCfWH6kbuV6awi04Uz3ebEAhzZ4ve1A2wg5CnLXdZC5Y7gwfVgbEgZSTmoYQSzC5OW4dfrjqiwApTACO6xoOL1AjWj6X6f6qFfF8TVmOzU9RhOd1N4QtzWI4fP6FYttNz5FuLdtYVXWVXH2Tf7I9fieMeWCHTMkM4VcmQyQHpbcP8MEb5f1g6Ckg5xk3HQr3wMBvQcOHpCPy1K8HCM7a5wkPDhgVA0BVmwNpsRIbDQZRtHK6dT6bGyalp6gbFZBuBHwD86gTzkrFY7HkOVrgc0gJcGJZe65Ce8v4Jn5OzkuVsiU8efm2Pw2RnbpWSAr7SkVdCwXK2XSJDQ5fZ4HBEz9VTFYrG23ELuLjvx5njOLNgDAJuf5JB2tn4nMjjcnl1e8qcYVwZqFzEv2zhLyDWMkV4tzl4asLnvyAxTBkxPRZj2pRABWwb3kEofpsHYxMTAn38YSpZreoXipZWBnu6HDURaruXaIPYFPYHl9Ls9wsuD7rzaGfbOyfVgLIGK5rODphwRA7lm88bGKY8b7tWOtepyEvaLxMI7GZF5ScwpZTYeEDNUKPzvM2Im9zehIaznpguNdNXNMLWnwPu4H6zEvajkw3G3ucSiXKmh6XNe3hkdSANm3vnxzRXm4fcuzAx68IElXE2bkGFElluDLo6EsUDWZ4JIWBVaDwYdJx8uCXbQdoifzCs5kuuClaDaDqIhb5hJ2WR8mxiueFsS0aDGdIYmye5svmNmzQxFmdOkHoF7CfwuU1yy4uEEt9vPSP2wFp1dyaMvJW68vtB4kddLmI6gIgVVcT6ZX1Qm6WsusPrdisPLB2ScodXojCbL3DLj6PKG8QDVMWTrL1TpafT2wslRledWIhsTlv2mI3C066WMcTSwKLXdEDhVvFJ6ShiLKSN7gnRrlE0BnAw";
        string salt = "saltKEYbcTcXHCBxtjD2PnBh44AIQ6XUOCESOhXpEp3HrcGMwbjzQKMSaf63IJemkURWoqHusIeVB8Il91NjiCGQacPUu9qTFaShLbKG0Yj4RCMV56WPj7E14EMpbxy6PlBdILBOkKUB6TGTPJXh1tpdOHTG6KuIvcbQp9qWjaf1uxAKgiTtYRIHhxjJI2viVa6fDZ67QOouOaf2RXQhpsWaTtAVnff6PIFcvJhdPDFGV5nvmZWoCZQodj6yXRDHPw9PyF0iLYm9uFtEunlAAxGB5qqea4X5tZvB1OfLVwymY3a3JPjdxTdvHxCHbqqE0zip61JNqdmeWxGtlRBC6CGoCiHO4XxHCntQBRJDcG0zW7joTdgtTBarsQQhlLXBGMNBSNmmTbDf3hFtawUBCJH18IAiRMwyeQJbJ2bERsY3MVRPuYCf4Au7gN72iGh1lRktSQtEFye7pO46kMXRrEjHQWXInMzzy7X2StXUzHVTFF2VdOoKn0WUqFNvB6PF7qIsOlYKj57bi1Psa34s85WxMSbTkhrd7VHdHZkTVaWdraohXYOePdeEvIwObCGEXkETUzqM5P2yzoBOJSdjpIYaa8zzdLD3yrb1TwCZuJVxsrq0XXY6vErU4QntsW0972XmGNyumFNJiPm4ONKh1RLvS1kddY3nm8276S4TUuZfrRQO8QxZRNuSaZI8JRZp5VojB5DktuMxAQkqoPjQ5Vtb6oXeOyY591CB1MEW1fLTCs0NrL321SaNRMqza1ETogAxpEiYwZ6pIgnMmSqNMRdZnCqA4gMWw1lIVATWK83OCeicNRUNOdfzS7A8vbLcmvKPtpOFvhNzwrrUdkvuKvaYJviQgeR7snGetO9JLCwIlHIj52gMCNU18d32SJl7Xomtl3wIe02SMvq1i1BcaX7lXioqWGmgVqBWU3fsUuGwHi6RUKCCQdEOBfNo2WdpFaCflcgnn0O6jVHCqkv8cQk81AqS00rAmHGCNTwyA6Tq5TXoLlDnC8gAQjDUsZp0z";
        u32 iters = 100000;
        u32 dklen = 63;

        string res = crypt::hashToString(
                crypt::pbkdf2(crypt::hmac_sha512,
                    vector<u8>(pass.c_str(), pass.c_str()+pass.length()),
                    vector<u8>(salt.c_str(), salt.c_str()+salt.length()),
                    iters, dklen));

        t.assert(res == "b8674f6c0cc9f8cf1f1874534fd5af01fc1504d76c2bc2aa0a75fe4dd5dfd1daf60ea7c85f122bceeb8772659d601231607726998eac3f6aab72eff7ba349f");
    }

    {
        string pass = "passDATAb00AB7YxDTT";
        string salt = "saltKEYbcTcXHCBxtjD";
        u32 iters = 1;
        u32 dklen = 65;

        string res = crypt::hashToString(
                crypt::pbkdf2(crypt::hmac_sha512,
                    vector<u8>(pass.c_str(), pass.c_str()+pass.length()),
                    vector<u8>(salt.c_str(), salt.c_str()+salt.length()),
                    iters, dklen));

        t.assert(res == "cbe6088ad4359af42e603c2a33760ef9d4017a7b2aad10af46f992c660a0b461ecb0dc2a79c2570941bea6a08d15d6887e79f32b132e1c134e9525eeddd744fa88");
    }

    {
        string pass = "passDATAb00AB7YxDTT";
        string salt = "saltKEYbcTcXHCBxtjD";
        u32 iters = 100000;
        u32 dklen = 65;

        string res = crypt::hashToString(
                crypt::pbkdf2(crypt::hmac_sha512,
                    vector<u8>(pass.c_str(), pass.c_str()+pass.length()),
                    vector<u8>(salt.c_str(), salt.c_str()+salt.length()),
                    iters, dklen));

        t.assert(res == "accdcd8798ae5cd85804739015ef2a11e32591b7b7d16f76819b30b0d49d80e1abea6c9822b80a1fdfe421e26f5603eca8a47a64c9a004fb5af8229f762ff41f7c");
    }

    {
        string pass = "passDATAb00AB7YxDTTl";
        string salt = "saltKEYbcTcXHCBxtjD2";
        u32 iters = 1;
        u32 dklen = 65;

        string res = crypt::hashToString(
                crypt::pbkdf2(crypt::hmac_sha512,
                    vector<u8>(pass.c_str(), pass.c_str()+pass.length()),
                    vector<u8>(salt.c_str(), salt.c_str()+salt.length()),
                    iters, dklen));

        t.assert(res == "8e5074a9513c1f1512c9b1df1d8bffa9d8b4ef9105dfc16681222839560fb63264bed6aabf761f180e912a66e0b53d65ec88f6a1519e14804eba6dc9df1370070b");
    }

    {
        string pass = "passDATAb00AB7YxDTTl";
        string salt = "saltKEYbcTcXHCBxtjD2";
        u32 iters = 100000;
        u32 dklen = 65;

        string res = crypt::hashToString(
                crypt::pbkdf2(crypt::hmac_sha512,
                    vector<u8>(pass.c_str(), pass.c_str()+pass.length()),
                    vector<u8>(salt.c_str(), salt.c_str()+salt.length()),
                    iters, dklen));

        t.assert(res == "594256b0bd4d6c9f21a87f7ba5772a791a10e6110694f44365cd94670e57f1aecd797ef1d1001938719044c7f018026697845eb9ad97d97de36ab8786aab5096e7");
    }

    {
        string pass = "passDATAb00AB7YxDTTlR";
        string salt = "saltKEYbcTcXHCBxtjD2P";
        u32 iters = 1;
        u32 dklen = 65;

        string res = crypt::hashToString(
                crypt::pbkdf2(crypt::hmac_sha512,
                    vector<u8>(pass.c_str(), pass.c_str()+pass.length()),
                    vector<u8>(salt.c_str(), salt.c_str()+salt.length()),
                    iters, dklen));

        t.assert(res == "a6ac8c048a7dfd7b838da88f22c3fab5bff15d7cb8d83a62c6721a8faf6903eab6152cb7421026e36f2ffef661eb4384dc276495c71b5cab72e1c1a38712e56b93");
    }

    {
        string pass = "passDATAb00AB7YxDTTlR";
        string salt = "saltKEYbcTcXHCBxtjD2P";
        u32 iters = 100000;
        u32 dklen = 65;

        string res = crypt::hashToString(
                crypt::pbkdf2(crypt::hmac_sha512,
                    vector<u8>(pass.c_str(), pass.c_str()+pass.length()),
                    vector<u8>(salt.c_str(), salt.c_str()+salt.length()),
                    iters, dklen));

        t.assert(res == "94ffc2b1a390b7b8a9e6a44922c330db2b193adcf082eecd06057197f35931a9d0ec0ee5c660744b50b61f23119b847e658d179a914807f4b8ab8eb9505af06526");
    }

    {
        string pass = "passDATAb00AB7YxDTTlRH2dqxDx19GDxDV1zFMz7E6QVqKIzwOtMnlxQLttpE5";
        string salt = "saltKEYbcTcXHCBxtjD2PnBh44AIQ6XUOCESOhXpEp3HrcGMwbjzQKMSaf63IJe";
        u32 iters = 1;
        u32 dklen = 65;

        string res = crypt::hashToString(
                crypt::pbkdf2(crypt::hmac_sha512,
                    vector<u8>(pass.c_str(), pass.c_str()+pass.length()),
                    vector<u8>(salt.c_str(), salt.c_str()+salt.length()),
                    iters, dklen));

        t.assert(res == "e2ccc7827f1dd7c33041a98906a8fd7bae1920a55fcb8f831683f14f1c3979351cb868717e5ab342d9a11acf0b12d3283931d609b06602da33f8377d1f1f9902da");
    }

    {
        string pass = "passDATAb00AB7YxDTTlRH2dqxDx19GDxDV1zFMz7E6QVqKIzwOtMnlxQLttpE5";
        string salt = "saltKEYbcTcXHCBxtjD2PnBh44AIQ6XUOCESOhXpEp3HrcGMwbjzQKMSaf63IJe";
        u32 iters = 100000;
        u32 dklen = 65;

        string res = crypt::hashToString(
                crypt::pbkdf2(crypt::hmac_sha512,
                    vector<u8>(pass.c_str(), pass.c_str()+pass.length()),
                    vector<u8>(salt.c_str(), salt.c_str()+salt.length()),
                    iters, dklen));

        t.assert(res == "07447401c85766e4aed583de2e6bf5a675eabe4f3618281c95616f4fc1fdfe6ecbc1c3982789d4fd941d6584ef534a78bd37ae02555d9455e8f089fdb4dfb6bb30");
    }

    {
        string pass = "passDATAb00AB7YxDTTlRH2dqxDx19GDxDV1zFMz7E6QVqKIzwOtMnlxQLttpE57";
        string salt = "saltKEYbcTcXHCBxtjD2PnBh44AIQ6XUOCESOhXpEp3HrcGMwbjzQKMSaf63IJem";
        u32 iters = 1;
        u32 dklen = 65;

        string res = crypt::hashToString(
                crypt::pbkdf2(crypt::hmac_sha512,
                    vector<u8>(pass.c_str(), pass.c_str()+pass.length()),
                    vector<u8>(salt.c_str(), salt.c_str()+salt.length()),
                    iters, dklen));

        t.assert(res == "b029a551117ff36977f283f579dc7065b352266ea243bdd3f920f24d4d141ed8b6e02d96e2d3bdfb76f8d77ba8f4bb548996ad85bb6f11d01a015ce518f9a71780");
    }

    {
        string pass = "passDATAb00AB7YxDTTlRH2dqxDx19GDxDV1zFMz7E6QVqKIzwOtMnlxQLttpE57";
        string salt = "saltKEYbcTcXHCBxtjD2PnBh44AIQ6XUOCESOhXpEp3HrcGMwbjzQKMSaf63IJem";
        u32 iters = 100000;
        u32 dklen = 65;

        string res = crypt::hashToString(
                crypt::pbkdf2(crypt::hmac_sha512,
                    vector<u8>(pass.c_str(), pass.c_str()+pass.length()),
                    vector<u8>(salt.c_str(), salt.c_str()+salt.length()),
                    iters, dklen));

        t.assert(res == "31f5cc83ed0e948c05a15735d818703aaa7bff3f09f5169caf5dba6602a05a4d5cff5553d42e82e40516d6dc157b8daeae61d3fea456d964cb2f7f9a63bbbdb59f");
    }

    {
        string pass = "passDATAb00AB7YxDTTlRH2dqxDx19GDxDV1zFMz7E6QVqKIzwOtMnlxQLttpE57U";
        string salt = "saltKEYbcTcXHCBxtjD2PnBh44AIQ6XUOCESOhXpEp3HrcGMwbjzQKMSaf63IJemk";
        u32 iters = 1;
        u32 dklen = 65;

        string res = crypt::hashToString(
                crypt::pbkdf2(crypt::hmac_sha512,
                    vector<u8>(pass.c_str(), pass.c_str()+pass.length()),
                    vector<u8>(salt.c_str(), salt.c_str()+salt.length()),
                    iters, dklen));

        t.assert(res == "28b8a9f644d6800612197bb74df460272e2276de8cc07ac4897ac24dbc6eb77499fcaf97415244d9a29da83fc347d09a5dbcfd6bd63ff6e410803dca8a900ab671");
    }

    {
        string pass = "passDATAb00AB7YxDTTlRH2dqxDx19GDxDV1zFMz7E6QVqKIzwOtMnlxQLttpE57U";
        string salt = "saltKEYbcTcXHCBxtjD2PnBh44AIQ6XUOCESOhXpEp3HrcGMwbjzQKMSaf63IJemk";
        u32 iters = 100000;
        u32 dklen = 65;

        string res = crypt::hashToString(
                crypt::pbkdf2(crypt::hmac_sha512,
                    vector<u8>(pass.c_str(), pass.c_str()+pass.length()),
                    vector<u8>(salt.c_str(), salt.c_str()+salt.length()),
                    iters, dklen));

        t.assert(res == "056bc9072a356b7d4da60dd66f5968c2caa375c0220eda6b47ef8e8d105ed68b44185fe9003fbba49e2c84240c9e8fd3f5b2f4f6512fd936450253db37d1002889");
    }

    {
        string pass = "passDATAb00AB7YxDTTlRH2dqxDx19GDxDV1zFMz7E6QVqKIzwOtMnlxQLttpE57Un4u12D2YD7oOPpiEvCDYvntXEe4NNPLCnGGeJArbYDEu6xDoCfWH6kbuV6awi0";
        string salt = "saltKEYbcTcXHCBxtjD2PnBh44AIQ6XUOCESOhXpEp3HrcGMwbjzQKMSaf63IJemkURWoqHusIeVB8Il91NjiCGQacPUu9qTFaShLbKG0Yj4RCMV56WPj7E14EMpbxy";
        u32 iters = 1;
        u32 dklen = 65;

        string res = crypt::hashToString(
                crypt::pbkdf2(crypt::hmac_sha512,
                    vector<u8>(pass.c_str(), pass.c_str()+pass.length()),
                    vector<u8>(salt.c_str(), salt.c_str()+salt.length()),
                    iters, dklen));

        t.assert(res == "16226c85e4f8d604573008bfe61c10b6947b53990450612dd4a3077f7dee2116229e68efd1df6d73bd3c6d07567790eea1e8b2ae9a1b046be593847d9441a1b766");
    }

    {
        string pass = "passDATAb00AB7YxDTTlRH2dqxDx19GDxDV1zFMz7E6QVqKIzwOtMnlxQLttpE57Un4u12D2YD7oOPpiEvCDYvntXEe4NNPLCnGGeJArbYDEu6xDoCfWH6kbuV6awi0";
        string salt = "saltKEYbcTcXHCBxtjD2PnBh44AIQ6XUOCESOhXpEp3HrcGMwbjzQKMSaf63IJemkURWoqHusIeVB8Il91NjiCGQacPUu9qTFaShLbKG0Yj4RCMV56WPj7E14EMpbxy";
        u32 iters = 100000;
        u32 dklen = 65;

        string res = crypt::hashToString(
                crypt::pbkdf2(crypt::hmac_sha512,
                    vector<u8>(pass.c_str(), pass.c_str()+pass.length()),
                    vector<u8>(salt.c_str(), salt.c_str()+salt.length()),
                    iters, dklen));

        t.assert(res == "70cf39f14c4caf3c81fa288fb46c1db52d19f72722f7bc84f040676d3371c89c11c50f69bcfbc3acb0ab9e92e4ef622727a916219554b2fa121bedda97ff3332ec");
    }

    {
        string pass = "passDATAb00AB7YxDTTlRH2dqxDx19GDxDV1zFMz7E6QVqKIzwOtMnlxQLttpE57Un4u12D2YD7oOPpiEvCDYvntXEe4NNPLCnGGeJArbYDEu6xDoCfWH6kbuV6awi04";
        string salt = "saltKEYbcTcXHCBxtjD2PnBh44AIQ6XUOCESOhXpEp3HrcGMwbjzQKMSaf63IJemkURWoqHusIeVB8Il91NjiCGQacPUu9qTFaShLbKG0Yj4RCMV56WPj7E14EMpbxy6";
        u32 iters = 1;
        u32 dklen = 65;

        string res = crypt::hashToString(
                crypt::pbkdf2(crypt::hmac_sha512,
                    vector<u8>(pass.c_str(), pass.c_str()+pass.length()),
                    vector<u8>(salt.c_str(), salt.c_str()+salt.length()),
                    iters, dklen));

        t.assert(res == "880c58c316d3a5b9f05977ab9c60c10abeebfad5ce89cae62905c1c4f80a0a098d82f95321a6220f8aeccfb45ce6107140899e8d655306ae6396553e2851376c57");
    }

    {
        string pass = "passDATAb00AB7YxDTTlRH2dqxDx19GDxDV1zFMz7E6QVqKIzwOtMnlxQLttpE57Un4u12D2YD7oOPpiEvCDYvntXEe4NNPLCnGGeJArbYDEu6xDoCfWH6kbuV6awi04";
        string salt = "saltKEYbcTcXHCBxtjD2PnBh44AIQ6XUOCESOhXpEp3HrcGMwbjzQKMSaf63IJemkURWoqHusIeVB8Il91NjiCGQacPUu9qTFaShLbKG0Yj4RCMV56WPj7E14EMpbxy6";
        u32 iters = 100000;
        u32 dklen = 65;

        string res = crypt::hashToString(
                crypt::pbkdf2(crypt::hmac_sha512,
                    vector<u8>(pass.c_str(), pass.c_str()+pass.length()),
                    vector<u8>(salt.c_str(), salt.c_str()+salt.length()),
                    iters, dklen));

        t.assert(res == "2668b71b3ca56136b5e87f30e098f6b4371cb5ed95537c7a073dac30a2d5be52756adf5bb2f4320cb11c4e16b24965a9c790def0cbc62906920b4f2eb84d1d4a30");
    }

    {
        string pass = "passDATAb00AB7YxDTTlRH2dqxDx19GDxDV1zFMz7E6QVqKIzwOtMnlxQLttpE57Un4u12D2YD7oOPpiEvCDYvntXEe4NNPLCnGGeJArbYDEu6xDoCfWH6kbuV6awi04U";
        string salt = "saltKEYbcTcXHCBxtjD2PnBh44AIQ6XUOCESOhXpEp3HrcGMwbjzQKMSaf63IJemkURWoqHusIeVB8Il91NjiCGQacPUu9qTFaShLbKG0Yj4RCMV56WPj7E14EMpbxy6P";
        u32 iters = 1;
        u32 dklen = 65;

        string res = crypt::hashToString(
                crypt::pbkdf2(crypt::hmac_sha512,
                    vector<u8>(pass.c_str(), pass.c_str()+pass.length()),
                    vector<u8>(salt.c_str(), salt.c_str()+salt.length()),
                    iters, dklen));

        t.assert(res == "93b9ba8283cc17d50ef3b44820828a258a996de258225d24fb59990a6d0de82dfb3fe2ac201952100e4cc8f06d883a9131419c0f6f5a6ecb8ec821545f14adf199");
    }

    {
        string pass = "passDATAb00AB7YxDTTlRH2dqxDx19GDxDV1zFMz7E6QVqKIzwOtMnlxQLttpE57Un4u12D2YD7oOPpiEvCDYvntXEe4NNPLCnGGeJArbYDEu6xDoCfWH6kbuV6awi04U";
        string salt = "saltKEYbcTcXHCBxtjD2PnBh44AIQ6XUOCESOhXpEp3HrcGMwbjzQKMSaf63IJemkURWoqHusIeVB8Il91NjiCGQacPUu9qTFaShLbKG0Yj4RCMV56WPj7E14EMpbxy6P";
        u32 iters = 100000;
        u32 dklen = 65;

        string res = crypt::hashToString(
                crypt::pbkdf2(crypt::hmac_sha512,
                    vector<u8>(pass.c_str(), pass.c_str()+pass.length()),
                    vector<u8>(salt.c_str(), salt.c_str()+salt.length()),
                    iters, dklen));

        t.assert(res == "2575b485afdf37c260b8f3386d33a60ed929993c9d48ac516ec66b87e06be54ade7e7c8cb3417c81603b080a8eefc56072811129737ced96236b9364e22ce3a542");
    }

    {
        string pass = "passDATAb00AB7YxDTTlRH2dqxDx19GDxDV1zFMz7E6QVqKIzwOtMnlxQLttpE57Un4u12D2YD7oOPpiEvCDYvntXEe4NNPLCnGGeJArbYDEu6xDoCfWH6kbuV6awi04Uz3ebEAhzZ4ve1A2wg5CnLXdZC5Y7gwfVgbEgZSTmoYQSzC5OW4dfrjqiwApTACO6xoOL1AjWj6X6f6qFfF8TVmOzU9RhOd1N4QtzWI4fP6FYttNz5FuLdtYVXWVXH2Tf7I9fieMeWCHTMkM4VcmQyQHpbcP8MEb5f1g6Ckg5xk3HQr3wMBvQcOHpCPy1K8HCM7a5wkPDhgVA0BVmwNpsRIbDQZRtHK6dT6bGyalp6gbFZBuBHwD86gTzkrFY7HkOVrgc0gJcGJZe65Ce8v4Jn5OzkuVsiU8efm2Pw2RnbpWSAr7SkVdCwXK2XSJDQ5fZ4HBEz9VTFYrG23ELuLjvx5njOLNgDAJuf5JB2tn4nMjjcnl1e8qcYVwZqFzEv2zhLyDWMkV4tzl4asLnvyAxTBkxPRZj2pRABWwb3kEofpsHYxMTAn38YSpZreoXipZWBnu6HDURaruXaIPYFPYHl9Ls9wsuD7rzaGfbOyfVgLIGK5rODphwRA7lm88bGKY8b7tWOtepyEvaLxMI7GZF5ScwpZTYeEDNUKPzvM2Im9zehIaznpguNdNXNMLWnwPu4H6zEvajkw3G3ucSiXKmh6XNe3hkdSANm3vnxzRXm4fcuzAx68IElXE2bkGFElluDLo6EsUDWZ4JIWBVaDwYdJx8uCXbQdoifzCs5kuuClaDaDqIhb5hJ2WR8mxiueFsS0aDGdIYmye5svmNmzQxFmdOkHoF7CfwuU1yy4uEEt9vPSP2wFp1dyaMvJW68vtB4kddLmI6gIgVVcT6ZX1Qm6WsusPrdisPLB2ScodXojCbL3DLj6PKG8QDVMWTrL1TpafT2wslRledWIhsTlv2mI3C066WMcTSwKLXdEDhVvFJ6ShiLKSN7gnRrlE0BnAw";
        string salt = "saltKEYbcTcXHCBxtjD2PnBh44AIQ6XUOCESOhXpEp3HrcGMwbjzQKMSaf63IJemkURWoqHusIeVB8Il91NjiCGQacPUu9qTFaShLbKG0Yj4RCMV56WPj7E14EMpbxy6PlBdILBOkKUB6TGTPJXh1tpdOHTG6KuIvcbQp9qWjaf1uxAKgiTtYRIHhxjJI2viVa6fDZ67QOouOaf2RXQhpsWaTtAVnff6PIFcvJhdPDFGV5nvmZWoCZQodj6yXRDHPw9PyF0iLYm9uFtEunlAAxGB5qqea4X5tZvB1OfLVwymY3a3JPjdxTdvHxCHbqqE0zip61JNqdmeWxGtlRBC6CGoCiHO4XxHCntQBRJDcG0zW7joTdgtTBarsQQhlLXBGMNBSNmmTbDf3hFtawUBCJH18IAiRMwyeQJbJ2bERsY3MVRPuYCf4Au7gN72iGh1lRktSQtEFye7pO46kMXRrEjHQWXInMzzy7X2StXUzHVTFF2VdOoKn0WUqFNvB6PF7qIsOlYKj57bi1Psa34s85WxMSbTkhrd7VHdHZkTVaWdraohXYOePdeEvIwObCGEXkETUzqM5P2yzoBOJSdjpIYaa8zzdLD3yrb1TwCZuJVxsrq0XXY6vErU4QntsW0972XmGNyumFNJiPm4ONKh1RLvS1kddY3nm8276S4TUuZfrRQO8QxZRNuSaZI8JRZp5VojB5DktuMxAQkqoPjQ5Vtb6oXeOyY591CB1MEW1fLTCs0NrL321SaNRMqza1ETogAxpEiYwZ6pIgnMmSqNMRdZnCqA4gMWw1lIVATWK83OCeicNRUNOdfzS7A8vbLcmvKPtpOFvhNzwrrUdkvuKvaYJviQgeR7snGetO9JLCwIlHIj52gMCNU18d32SJl7Xomtl3wIe02SMvq1i1BcaX7lXioqWGmgVqBWU3fsUuGwHi6RUKCCQdEOBfNo2WdpFaCflcgnn0O6jVHCqkv8cQk81AqS00rAmHGCNTwyA6Tq5TXoLlDnC8gAQjDUsZp0z";
        u32 iters = 1;
        u32 dklen = 65;

        string res = crypt::hashToString(
                crypt::pbkdf2(crypt::hmac_sha512,
                    vector<u8>(pass.c_str(), pass.c_str()+pass.length()),
                    vector<u8>(salt.c_str(), salt.c_str()+salt.length()),
                    iters, dklen));

        t.assert(res == "384bcd6914407e40c295d1037cf4f990e8f0e720af43cb706683177016d36d1a14b3a7cf22b5df8d5d7d44d69610b64251ade2e7ab54a3813a89935592e391bf91");
    }

    {
        // Disabled because it takes too long to run.
//         string pass = "passDATA";
//         string salt = "saltKEYbc";
//         u32 iters = 16777216;
//         u32 dklen = 7;
//
//         string res = crypt::hashToString(
//                 crypt::pbkdf2(crypt::hmac_sha512,
//                     vector<u8>(pass.c_str(), pass.c_str()+pass.length()),
//                     vector<u8>(salt.c_str(), salt.c_str()+salt.length()),
//                     iters, dklen));
//
//         t.assert(res == "ab96c76400d08b");
    }

    {
        // Disabled because it takes too long to run.
//         string pass = "passDATAb00AB";
//         string salt = "saltKEYbcTcX";
//         u32 iters = 2097152;
//         u32 dklen = 481;
//
//         string res = crypt::hashToString(
//                 crypt::pbkdf2(crypt::hmac_sha512,
//                     vector<u8>(pass.c_str(), pass.c_str()+pass.length()),
//                     vector<u8>(salt.c_str(), salt.c_str()+salt.length()),
//                     iters, dklen));
//
//         t.assert(res == "c8cb4b4b498b32cde191159866a8e86b4c9d84ef1d0a37cf7b9bdc7872edd5f02242aa7d83172c778ef64c788d622acbcd4317c4b63a2ede184cb2a5f6b94815c395cc822d68c637adb0e928c9692d32d6b66b3825cdb6ac9b57d9d15bca72cc32773ca45350bb460f83172b75edd418e2c39df437fffddef6ff5e83afc2974e5b391303c80b73da815e979118fb41acc3e2019db30c14650dc7e75d67a048541563a3eca996cf15f9b3dd7c768b45613078cf772292f092ccfec10f027669d60edf56a383894f0efd7ddc3551e1c6aa366f7efb39981bf0bdf7894a83d051e900af2fb81ca990f52ee613a5c2d28d28683e331f50bd10b6f8af12705e505bca3bb0d3869246863387dd385748718b3aaa51ba12bb067f1abd6b8f2e0decda0a6693d1331349470e78212b2b4700709bc22c86ae7adab9c74635bc0e40a18be604b8be7ed1e0419258bb0c38d27264783fe2a915cd63c7cbb6c2d937803d86ffe9dc58132f2af7642c782af6a0d50ab47622a73ef16618e15b5ce8eee9f5a1a477a02adb5e95638792811013a9a8acc9f618c4726dc26e67c1ddce6e1e90594c94d4de8fd8d89400ab3e8138089b4cd5893bd66691708d1c27ff7e69f12d1a15983352933de1583a2127dc8b62e345c0b1cd14f9f7bc85ffbceb40e80e84e8e8c0");
    }
}


int main()
{
    tCrashReporter::init();
    srand(time(0));

    tTest("tMD5 test", testMD5, kNumIters);
    tTest("tSHA1 test", testSHA1, kNumIters);
    tTest("tSHA224 test", testSHA224, kNumIters);
    tTest("tSHA256 test", testSHA256, kNumIters);
    tTest("tSHA384 test", testSHA384, kNumIters);
    tTest("tSHA512 test", testSHA512, kNumIters);
    tTest("tWhirlpool test", testWhirlpool, kNumIters);

    tTest("HMAC_MD5 test", testHMAC_MD5, kNumIters);
    tTest("HMAC_SHA1 test", testHMAC_SHA1, kNumIters);
    tTest("HMAC_SHA224 test", testHMAC_SHA224, kNumIters);
    tTest("HMAC_SHA256 test", testHMAC_SHA256, kNumIters);
    tTest("HMAC_SHA384 test", testHMAC_SHA384, kNumIters);
    tTest("HMAC_SHA512 test", testHMAC_SHA512, kNumIters);
    tTest("HMAC_WHIRLPOOL test", testHMAC_WHIRLPOOL, kNumIters);

    tTest("PBKDF2_HMAC_SHA1 test", testPBKDF2_HMAC_SHA1);
    tTest("PBKDF2_HMAC_SHA512 test", testPBKDF2_HMAC_SHA512);

    return 0;
}
