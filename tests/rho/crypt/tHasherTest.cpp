#include <rho/crypt/hash_utils.h>
#include <rho/crypt/tMD5.h>
#include <rho/crypt/tSHA0.h>
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

    tests.push_back(make_pair(make_pair(string("key"),string("The quick brown fox jumps over the lazy dog")),
                              string("88ff8b54675d39b8f72322e65ff945c52d96379988ada25639747e69")));

    tests.push_back(make_pair(make_pair(string("asd;fkjasdfasdf9sadf9asdfkljd;fakjsdf;jasd;fkjasdfahsdfkals;jdf;aksdjf;aksdfj;aksjdfasdfereirerjasdfkjasdfasdfasdf"),string("asdjfasdfasdkfja;sdkfjasd9fasd908fas9d08fa0sdfasdfnmvhn,zxmvnskldfhsdofiudsf9uasdfadksfj;askdfj;asdkfhajksherfernamsdbnfasdfjughaesuiryhas;dkfhnas;dfiha;lskdhfa;hfa;sdkjhfa;sdkfha;sdkfhasd;fhernase.rhka.seklrbha.kdifha;lsdifyha;.erkih>Sdklea;lsdifhas d;fakj sd;fka sdfjhlaskjhfajsd;hfhasleufhajerbalsuer;lasker;aseirya;eriyha;sekhrfa;keiryha;so8eryawerkjane.resuihfD:SofiZeihf;awirha/.iwhef.zakdehfa;wiery;KHFA;SDKFH;h;kjadhf;adhksf")),
                              string("417b31f48cfb6fc98cf53497e8eefb5d29a84238c91519a05cdb4c44")));

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

    tests.push_back(make_pair(make_pair(string("key"),string("The quick brown fox jumps over the lazy dog")),
                              string("d7f4727e2c0b39ae0f1e40cc96f60242d5b7801841cea6fc592c5d3e1ae50700582a96cf35e1e554995fe4e03381c237")));

    tests.push_back(make_pair(make_pair(string("asd;fkjasdfasdf9sadf9asdfkljd;fakjsdf;jasd;fkjasdfahsdfkals;jdf;aksdjf;aksdfj;aksjdfasdfereirerjasdfkjasdfasdfasdf"),string("asdjfasdfasdkfja;sdkfjasd9fasd908fas9d08fa0sdfasdfnmvhn,zxmvnskldfhsdofiudsf9uasdfadksfj;askdfj;asdkfhajksherfernamsdbnfasdfjughaesuiryhas;dkfhnas;dfiha;lskdhfa;hfa;sdkjhfa;sdkfha;sdkfhasd;fhernase.rhka.seklrbha.kdifha;lsdifyha;.erkih>Sdklea;lsdifhas d;fakj sd;fka sdfjhlaskjhfajsd;hfhasleufhajerbalsuer;lasker;aseirya;eriyha;sekhrfa;keiryha;so8eryawerkjane.resuihfD:SofiZeihf;awirha/.iwhef.zakdehfa;wiery;KHFA;SDKFH;h;kjadhf;adhksf")),
                              string("f800902fa3f7e69e8f132069b5184ea34cfb0c8a72822edb7c08ceb05e8a33d393bb7d56846cb5b947cada326bbe6e66")));

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

    tests.push_back(make_pair(make_pair(string("key"),string("The quick brown fox jumps over the lazy dog")),
                              string("b42af09057bac1e2d41708e48a902e09b5ff7f12ab428a4fe86653c73dd248fb82f948a549f7b791a5b41915ee4d1ec3935357e4e2317250d0372afa2ebeeb3a")));

    tests.push_back(make_pair(make_pair(string("asd;fkjasdfasdf9sadf9asdfkljd;fakjsdf;jasd;fkjasdfahsdfkals;jdf;aksdjf;aksdfj;aksjdfasdfereirerjasdfkjasdfasdfasdf"),string("asdjfasdfasdkfja;sdkfjasd9fasd908fas9d08fa0sdfasdfnmvhn,zxmvnskldfhsdofiudsf9uasdfadksfj;askdfj;asdkfhajksherfernamsdbnfasdfjughaesuiryhas;dkfhnas;dfiha;lskdhfa;hfa;sdkjhfa;sdkfha;sdkfhasd;fhernase.rhka.seklrbha.kdifha;lsdifyha;.erkih>Sdklea;lsdifhas d;fakj sd;fka sdfjhlaskjhfajsd;hfhasleufhajerbalsuer;lasker;aseirya;eriyha;sekhrfa;keiryha;so8eryawerkjane.resuihfD:SofiZeihf;awirha/.iwhef.zakdehfa;wiery;KHFA;SDKFH;h;kjadhf;adhksf")),
                              string("c689bde6ab4a3805ed3d90e77635b01b95b03697d3201116d86dca49fced98ae4ceee4936f4fe02c99a14e5d3e77e23d1b2e0749b2bb0aadf08ef5189a436aa6")));

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


int main()
{
    tCrashReporter::init();
    srand(time(0));

    tTest("tMD5 test", testMD5, kNumIters);
    tTest("tSHA0 test", testSHA0, kNumIters);
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

    return 0;
}
