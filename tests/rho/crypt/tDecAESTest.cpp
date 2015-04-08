#include <rho/crypt/tDecAES.h>
#include <rho/tCrashReporter.h>
#include <rho/tTest.h>

#include <string>
#include <vector>


using namespace rho;
using std::string;
using std::vector;


static const u32 kTestIters = 2000;

static bool gCanTestFastASM = true;


int hexCharToValue(char hex)
{
    if (hex >= '0' && hex <= '9')
        return hex-'0';
    if (hex >= 'a' && hex <= 'f')
        return 10 + hex-'a';
    if (hex >= 'A' && hex <= 'F')
        return 10 + hex-'A';
    throw std::runtime_error("...?");
}


vector<u8> hexStringToVector(string hexString)
{
    vector<u8> vect;
    for (size_t i = 0; (i+1) < hexString.length(); i += 2)
        vect.push_back(16 * hexCharToValue(hexString[i]) + hexCharToValue(hexString[i+1]));
    return vect;
}


crypt::nKeyLengthAES getKeyLengthForKey(vector<u8> key)
{
    if (key.size() == 16)
        return crypt::k128bit;
    if (key.size() == 24)
        return crypt::k192bit;
    if (key.size() == 32)
        return crypt::k256bit;
    throw std::runtime_error("...?");
}


void test_ecb(const tTest& t, string key,
                              string pt,
                              string ct)
{
    // Fast ASM impl:
    if (gCanTestFastASM)
    {
        vector<u8> ptbuf = hexStringToVector(pt);
        vector<u8> ctbuf = hexStringToVector(ct);
        t.assert(ptbuf.size() == 16);
        t.assert(ctbuf.size() == 16);

        vector<u8> keybuf = hexStringToVector(key);
        crypt::tDecAES aes(crypt::kOpModeECB, &keybuf[0], getKeyLengthForKey(keybuf), true);

        u8 outbuf[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
        aes.dec(&ctbuf[0], outbuf, 1);

        for (int i = 0; i < 16; i++)
            t.assert(outbuf[i] == ptbuf[i]);
    }

    // Fallback impl:
    {
        vector<u8> ptbuf = hexStringToVector(pt);
        vector<u8> ctbuf = hexStringToVector(ct);
        t.assert(ptbuf.size() == 16);
        t.assert(ctbuf.size() == 16);

        vector<u8> keybuf = hexStringToVector(key);
        crypt::tDecAES aes(crypt::kOpModeECB, &keybuf[0], getKeyLengthForKey(keybuf), false);

        u8 outbuf[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
        aes.dec(&ctbuf[0], outbuf, 1);

        for (int i = 0; i < 16; i++)
            t.assert(outbuf[i] == ptbuf[i]);
    }
}


void test_cbc(const tTest& t, string key,
                              string iv,
                              string pt,
                              string ct)
{
    // Fast ASM impl:
    if (gCanTestFastASM)
    {
        vector<u8> ptbuf = hexStringToVector(pt);
        vector<u8> ctbuf = hexStringToVector(ct);
        t.assert(ptbuf.size() == 16);
        t.assert(ctbuf.size() == 16);

        vector<u8> keybuf = hexStringToVector(key);
        crypt::tDecAES aes(crypt::kOpModeCBC, &keybuf[0], getKeyLengthForKey(keybuf), true);

        vector<u8> ivbuf = hexStringToVector(iv);
        t.assert(ivbuf.size() == 16);

        u8 outbuf[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
        aes.dec(&ctbuf[0], outbuf, 1, &ivbuf[0]);

        for (int i = 0; i < 16; i++)
            t.assert(outbuf[i] == ptbuf[i]);
    }

    // Fallback impl:
    {
        vector<u8> ptbuf = hexStringToVector(pt);
        vector<u8> ctbuf = hexStringToVector(ct);
        t.assert(ptbuf.size() == 16);
        t.assert(ctbuf.size() == 16);

        vector<u8> keybuf = hexStringToVector(key);
        crypt::tDecAES aes(crypt::kOpModeCBC, &keybuf[0], getKeyLengthForKey(keybuf), false);

        vector<u8> ivbuf = hexStringToVector(iv);
        t.assert(ivbuf.size() == 16);

        u8 outbuf[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
        aes.dec(&ctbuf[0], outbuf, 1, &ivbuf[0]);

        for (int i = 0; i < 16; i++)
            t.assert(outbuf[i] == ptbuf[i]);
    }
}


void test128(const tTest& t)
{
    test_ecb(t, "2b7e151628aed2a6abf7158809cf4f3c", "6bc1bee22e409f96e93d7e117393172a", "3ad77bb40d7a3660a89ecaf32466ef97");
    test_ecb(t, "2b7e151628aed2a6abf7158809cf4f3c", "ae2d8a571e03ac9c9eb76fac45af8e51", "f5d3d58503b9699de785895a96fdbaaf");
    test_ecb(t, "2b7e151628aed2a6abf7158809cf4f3c", "30c81c46a35ce411e5fbc1191a0a52ef", "43b1cd7f598ece23881b00e3ed030688");
    test_ecb(t, "2b7e151628aed2a6abf7158809cf4f3c", "f69f2445df4f9b17ad2b417be66c3710", "7b0c785e27e8ad3f8223207104725dd4");

    test_cbc(t, "2b7e151628aed2a6abf7158809cf4f3c", "000102030405060708090A0B0C0D0E0F", "6bc1bee22e409f96e93d7e117393172a", "7649abac8119b246cee98e9b12e9197d");
    test_cbc(t, "2b7e151628aed2a6abf7158809cf4f3c", "7649ABAC8119B246CEE98E9B12E9197D", "ae2d8a571e03ac9c9eb76fac45af8e51", "5086cb9b507219ee95db113a917678b2");
    test_cbc(t, "2b7e151628aed2a6abf7158809cf4f3c", "5086CB9B507219EE95DB113A917678B2", "30c81c46a35ce411e5fbc1191a0a52ef", "73bed6b8e3c1743b7116e69e22229516");
    test_cbc(t, "2b7e151628aed2a6abf7158809cf4f3c", "73BED6B8E3C1743B7116E69E22229516", "f69f2445df4f9b17ad2b417be66c3710", "3ff1caa1681fac09120eca307586e1a7");
}


void test192(const tTest& t)
{
    test_ecb(t, "8e73b0f7da0e6452c810f32b809079e562f8ead2522c6b7b", "6bc1bee22e409f96e93d7e117393172a", "bd334f1d6e45f25ff712a214571fa5cc");
    test_ecb(t, "8e73b0f7da0e6452c810f32b809079e562f8ead2522c6b7b", "ae2d8a571e03ac9c9eb76fac45af8e51", "974104846d0ad3ad7734ecb3ecee4eef");
    test_ecb(t, "8e73b0f7da0e6452c810f32b809079e562f8ead2522c6b7b", "30c81c46a35ce411e5fbc1191a0a52ef", "ef7afd2270e2e60adce0ba2face6444e");
    test_ecb(t, "8e73b0f7da0e6452c810f32b809079e562f8ead2522c6b7b", "f69f2445df4f9b17ad2b417be66c3710", "9a4b41ba738d6c72fb16691603c18e0e");

    test_cbc(t, "8e73b0f7da0e6452c810f32b809079e562f8ead2522c6b7b", "000102030405060708090A0B0C0D0E0F", "6bc1bee22e409f96e93d7e117393172a", "4f021db243bc633d7178183a9fa071e8");
    test_cbc(t, "8e73b0f7da0e6452c810f32b809079e562f8ead2522c6b7b", "4F021DB243BC633D7178183A9FA071E8", "ae2d8a571e03ac9c9eb76fac45af8e51", "b4d9ada9ad7dedf4e5e738763f69145a");
    test_cbc(t, "8e73b0f7da0e6452c810f32b809079e562f8ead2522c6b7b", "B4D9ADA9AD7DEDF4E5E738763F69145A", "30c81c46a35ce411e5fbc1191a0a52ef", "571b242012fb7ae07fa9baac3df102e0");
    test_cbc(t, "8e73b0f7da0e6452c810f32b809079e562f8ead2522c6b7b", "571B242012FB7AE07FA9BAAC3DF102E0", "f69f2445df4f9b17ad2b417be66c3710", "08b0e27988598881d920a9e64f5615cd");
}


void test256(const tTest& t)
{
    test_ecb(t, "603deb1015ca71be2b73aef0857d77811f352c073b6108d72d9810a30914dff4", "6bc1bee22e409f96e93d7e117393172a", "f3eed1bdb5d2a03c064b5a7e3db181f8");
    test_ecb(t, "603deb1015ca71be2b73aef0857d77811f352c073b6108d72d9810a30914dff4", "ae2d8a571e03ac9c9eb76fac45af8e51", "591ccb10d410ed26dc5ba74a31362870");
    test_ecb(t, "603deb1015ca71be2b73aef0857d77811f352c073b6108d72d9810a30914dff4", "30c81c46a35ce411e5fbc1191a0a52ef", "b6ed21b99ca6f4f9f153e7b1beafed1d");
    test_ecb(t, "603deb1015ca71be2b73aef0857d77811f352c073b6108d72d9810a30914dff4", "f69f2445df4f9b17ad2b417be66c3710", "23304b7a39f9f3ff067d8d8f9e24ecc7");

    test_cbc(t, "603deb1015ca71be2b73aef0857d77811f352c073b6108d72d9810a30914dff4", "000102030405060708090A0B0C0D0E0F", "6bc1bee22e409f96e93d7e117393172a", "f58c4c04d6e5f1ba779eabfb5f7bfbd6");
    test_cbc(t, "603deb1015ca71be2b73aef0857d77811f352c073b6108d72d9810a30914dff4", "F58C4C04D6E5F1BA779EABFB5F7BFBD6", "ae2d8a571e03ac9c9eb76fac45af8e51", "9cfc4e967edb808d679f777bc6702c7d");
    test_cbc(t, "603deb1015ca71be2b73aef0857d77811f352c073b6108d72d9810a30914dff4", "9CFC4E967EDB808D679F777BC6702C7D", "30c81c46a35ce411e5fbc1191a0a52ef", "39f23369a9d9bacfa530e26304231461");
    test_cbc(t, "603deb1015ca71be2b73aef0857d77811f352c073b6108d72d9810a30914dff4", "39F23369A9D9BACFA530E26304231461", "f69f2445df4f9b17ad2b417be66c3710", "b2eb05e2c39be9fcda6c19078c6a9d1b");
}


int main()
{
    tCrashReporter::init();
    srand(time(0));

    // Test vectors are from:
    // http://www.inconteam.com/software-development/41-encryption/55-aes-test-vectors

    if (!crypt::tDecAES::canRunFastASM())
    {
        std::cout << "Cannot run fast ASM instructions on this machine!" << std::endl;
        gCanTestFastASM = false;
    }

    tTest("tDecAES 128bit test", test128, kTestIters);
    tTest("tDecAES 192bit test", test192, kTestIters);
    tTest("tDecAES 256bit test", test256, kTestIters);

    return 0;
}
