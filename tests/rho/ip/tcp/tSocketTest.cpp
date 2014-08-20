#include <rho/ip/tcp/tSocket.h>
#include <rho/tCrashReporter.h>
#include <rho/tTest.h>
#include <rho/sync/tTimer.h>

#include <iostream>
#include <string>


using namespace rho;
using std::cout;
using std::endl;


void connectTest(const tTest& t)
{
    ip::tcp::tSocket("google.com", 80);
}

void exceptionTest(const tTest& t)
{
    try
    {
        ip::tAddrGroup g(ip::tAddrGroup::kLocalhostConnect);
        ip::tcp::tSocket(g, 34);
        t.fail();
    }
    catch (ip::eHostUnreachableError& e) { }
}

void timeoutTest(const tTest& t, u32 correctTimeoutMS, u32 allowance)
{
    bool didThrow = false;
    u64 starttime = sync::tTimer::usecTime();
    try
    {
        ip::tcp::tSocket("10.0.0.1", 34, correctTimeoutMS);
        t.fail();
    }
    catch (ip::eHostUnreachableError& e)
    {
        u64 endtime = sync::tTimer::usecTime();
        u64 elapsed = endtime - starttime;
        u64 elapsedMS = elapsed / 1000;

        if (elapsedMS > correctTimeoutMS+allowance || elapsedMS < correctTimeoutMS-allowance)
        {
            std::cerr << "elapsedMS: " << elapsedMS << std::endl;
            std::cerr << "expectedMS: " << correctTimeoutMS << std::endl;
            t.fail();
        }

        didThrow = true;
    }
    t.assert(didThrow);
}

void timeoutTest(const tTest& t)
{
    timeoutTest(t,   50, 40);
    timeoutTest(t,  100, 50);
    timeoutTest(t,  200, 100);
    timeoutTest(t,  500, 100);
    //timeoutTest(t, 1000, 50);
    //timeoutTest(t, 2000, 50);
}

int main()
{
    tCrashReporter::init();

    tTest("connect test", connectTest);
    tTest("exception test", exceptionTest);
    tTest("connect timeout test", timeoutTest);

    return 0;
}
