#include <rho/crypt/tRSA.h>
#include <rho/tCrashReporter.h>
#include <rho/tTest.h>

#include <iostream>
#include <fstream>
#include <vector>


using namespace rho;
using std::cout;
using std::endl;
using std::vector;


static const int kNumIters = 10;


void print(const vector<u8>& bytes)
{
    for (int i = (int)bytes.size()-1; i >= 0; i--)
        cout << " " << (u32)bytes[i];
    cout << endl;
}


void test(const tTest& t, const crypt::tRSA& rsa)
{
    //cout << "n = " << rsa.getModulus() << endl;
    //cout << "e = " << rsa.getPubKey() << endl;
    //cout << "d = " << rsa.getPrivKey() << endl;

    for (int k = 0; k < kNumIters; k++)
    {
        int messageLen = (rand() % rsa.maxMessageLength()) + 1;
        vector<u8> origMessage;
        for (int i = 0; i < messageLen; i++)
            origMessage.push_back(rand() % 256);

        vector<u8> cypherMessage = rsa.encrypt(origMessage);

        vector<u8> recoveredMessage = rsa.decrypt(cypherMessage);

        t.assert(origMessage == recoveredMessage);

        //cout << "." << std::flush;
    }
    //cout << endl;
}


void testWithFile(const tTest& t)
{
    std::ifstream file("tests/rho/crypt/rsakeys.txt");
    std::string n, e, d;
    file >> n >> e >> d;
    crypt::tRSA rsa(n, e, d);
    test(t, rsa);
}


void testWithGenerate(const tTest& t)
{
    crypt::tRSA rsa = crypt::tRSA::generate(1024, 50);
    test(t, rsa);
}


int main()
{
    tCrashReporter::init();
    srand(time(0));

    tTest("tRSA test with file", testWithFile);
    tTest("tRSA test with generate", testWithGenerate);

    return 0;
}
