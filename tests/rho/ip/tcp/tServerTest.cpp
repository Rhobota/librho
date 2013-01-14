#include <rho/ip/tcp/tServer.h>
#include <rho/tCrashReporter.h>
#include <rho/tTest.h>

#include <iostream>
#include <string>


using namespace rho;
using std::cout;
using std::endl;


void localhostBindTest(const tTest& t)
{
    {
        // When no tAddrGroup is given, the server is bound to
        // the localhost address.
        ip::tcp::tServer server(8571);
        t.iseq(server.getBindAddress().toString(), "::1");
        t.iseq(server.getBindAddress().getVersion(), ip::kIPv6);
        t.iseq(server.getBindPort(), 8571);
    }

    {
        // Or you can be explicit about binding to the localhost address.
        ip::tAddrGroup g(ip::tAddrGroup::kLocalhostBind);
        ip::tcp::tServer server(g, 8572);
        t.iseq(server.getBindAddress().toString(), "::1");
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

int main()
{
    tCrashReporter::init();

    tTest("localhost bind test", localhostBindTest);
    tTest("exception test", exceptionTest);

    return 0;
}
