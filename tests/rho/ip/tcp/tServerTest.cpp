#include <rho/ip/tcp/tServer.h>
#include <rho/tCrashReporter.h>
#include <rho/tTest.h>
#include <rho/sync/tTimer.h>

#include <iostream>
#include <string>


using namespace rho;
using std::string;


void localhostBindTest(const tTest& t)
{
    {
        // When no tAddrGroup is given, the server is bound to
        // the localhost address.
        ip::tcp::tServer server(8571);
        string str = server.getBindAddress().toString();
        t.assert(str == "::1");
        t.iseq(server.getBindAddress().getVersion(), ip::kIPv6);
        t.iseq(server.getBindPort(), 8571);
    }

    {
        // Or you can be explicit about binding to the localhost address.
        ip::tAddrGroup g(ip::tAddrGroup::kLocalhostBind);
        ip::tcp::tServer server(g, 8572);
        string str = server.getBindAddress().toString();
        t.assert(str == "::1");
        t.iseq(server.getBindAddress().getVersion(), ip::kIPv6);
        t.iseq(server.getBindPort(), 8572);
    }
}

void exceptionTest(const tTest& t)
{
    bool boundOnce = false;
    try
    {
        ip::tcp::tServer server1(2013);
        boundOnce = true;
        ip::tcp::tServer server2(2013);
        t.fail();
    }
    catch (ip::eSocketBindError& e) { }
    t.assert(boundOnce);
}

void timeoutTest(const tTest& t, u32 correctTimeoutMS, u32 allowance)
{
    ip::tcp::tServer server(12345);

    u64 starttime = sync::tTimer::usecTime();

    refc<ip::tcp::tSocket> socket = server.accept(correctTimeoutMS);
    t.assert(socket == NULL);

    u64 endtime = sync::tTimer::usecTime();
    u64 elapsed = endtime - starttime;
    u64 elapsedMS = elapsed / 1000;

    if (elapsedMS > correctTimeoutMS+allowance || elapsedMS < correctTimeoutMS-allowance)
    {
        std::cerr << "elapsedMS: " << elapsedMS << std::endl;
        std::cerr << "expectedMS: " << correctTimeoutMS << std::endl;
        t.fail();
    }
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

    tTest("localhost bind test", localhostBindTest);
    tTest("exception test", exceptionTest);
    tTest("timeout test", timeoutTest);

    return 0;
}
