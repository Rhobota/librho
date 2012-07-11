#include "rho/tTest.h"
#include "rho/tCrashReporter.h"

#include <iostream>
#include <sstream>
#include <string>


using namespace rho;
using std::cin;
using std::cout;
using std::cerr;
using std::endl;
using std::string;


void passingTest1(const tTest& t)
{
    t.iseq(2+2, 4);
    t.iseq(1, true);
    t.noteq(1, false);
    t.assert(true);
    t.reject(false);
}

void passingTest2(const tTest& t)
{
    string s1 = "Bla bla";
    string s2 = "Bla ";
    string s3 = "bla";

    t.noteq(s1, s2);
    t.noteq(s1, s3);
    t.noteq(s2, s3);
    t.iseq(s1, s2 + s3);

    t.reject(s1 == s2);       //
    t.reject(s1 == s3);       // These would be better as iseq()/noteq() calls,
    t.reject(s2 == s3);       // but I'm using assert()/reject() as examples.
    t.assert(s1 == s2 + s3);  //
}

void failingTest1(const tTest& t)
{
    t.iseq(2+2, 5);
}

void failingTest2(const tTest& t)
{
    t.noteq(2+2, 4);
}

void failingTest3(const tTest& t)
{
    t.assert(2+2 == 5);
}

void failingTest4(const tTest& t)
{
    t.reject(2+2 == 4);
}

int main()
{
    tCrashReporter::init();

    tTest::testfunc passingTestFuncs[] = {
        passingTest1,
        passingTest2
    };
    int numPassingTestFuncs = sizeof(passingTestFuncs)/sizeof(tTest::testfunc);

    tTest::testfunc failingTestFuncs[] = {
        failingTest1,
        failingTest2,
        failingTest3,
        failingTest4
    };
    int numFailingTestFuncs = sizeof(failingTestFuncs)/sizeof(tTest::testfunc);

    for (int i = 0; i < numPassingTestFuncs; i++)
    {
        std::ostringstream o;
        o << "Test " << i;
        tTest(o.str(), passingTestFuncs[i]);
    }

    for (int i = 0; i < numFailingTestFuncs; i++)
    {
        try
        {
            tTest("Bad Test", failingTestFuncs[i]);
            cerr << "Oh no, a bad test passed for some reason..." << endl;
            exit(1);
        }
        catch (tTest::eTestFailedError& e)
        {
            cout << "Test " << i + numPassingTestFuncs
                 << " failed, as was expected." << endl;
        }
        catch (...)
        {
            cerr << "An unknown exception was thrown!" << endl;
            exit(1);
        }
    }

    return 0;
}
