#include "rho/ip/tcp/tSocket.h"
#include "rho/tCrashReporter.h"
#include "rho/tTest.h"

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

int main()
{
    tCrashReporter::init();

    tTest("connect test", connectTest);
    tTest("exception test", exceptionTest);

    return 0;
}
