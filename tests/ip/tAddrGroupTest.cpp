#include "rho/ip/tAddrGroup.h"
#include "rho/tCrashReporter.h"
#include "rho/tTest.h"

#include <iostream>


using namespace rho;
using std::cout;
using std::endl;


void localhostTest(const tTest& t)
{
    ip::tAddrGroup localhostGroup(ip::tAddrGroup::kLocalhostConnect);
    t.iseq(localhostGroup.size(), 1);
    t.iseq(localhostGroup[0].toString(), "::1");
    t.iseq(localhostGroup[0].getVersion(), ip::kIPv6);
}

void ipStringTest(const tTest& t)
{
    {
        std::string ipStr = "2.3.4.5";
        ip::tAddrGroup someGroup(ipStr, false);
        t.iseq(someGroup.size(), 1);
        t.iseq(someGroup[0].toString(), ipStr);
        t.iseq(someGroup[0].getVersion(), ip::kIPv4);
    }

    {
        std::string ipStr = "1122:3344:5566:7788:99aa:bbcc:ddee:11ff";
        ip::tAddrGroup someGroup(ipStr, false);
        t.iseq(someGroup.size(), 1);
        t.iseq(someGroup[0].toString(), ipStr);
        t.iseq(someGroup[0].getVersion(), ip::kIPv6);
    }

    {
        std::string ipStr = "2.3.4.5";
        ip::tAddrGroup someGroup(ipStr, true);
        t.iseq(someGroup.size(), 1);
        t.iseq(someGroup[0].toString(), ipStr);
        t.iseq(someGroup[0].getVersion(), ip::kIPv4);
    }

    {
        std::string ipStr = "1122:3344:5566:7788:99aa:bbcc:ddee:11ff";
        ip::tAddrGroup someGroup(ipStr, true);
        t.iseq(someGroup.size(), 1);
        t.iseq(someGroup[0].toString(), ipStr);
        t.iseq(someGroup[0].getVersion(), ip::kIPv6);
    }
}

void hostnameStringTest(const tTest& t)
{
    {
        std::string hostName = "rhobota.com";
        ip::tAddrGroup hostGroup(hostName);       // will set resolve==true
        t.assert(hostGroup.size() > 0);
        for (int i = 0; i < hostGroup.size(); i++)
            ip::tAddr a = hostGroup[i];
    }

    {
        std::string hostName = "ryanserver.com";
        ip::tAddrGroup hostGroup(hostName, true); // explicitly setting resolve
        t.assert(hostGroup.size() > 0);
        for (int i = 0; i < hostGroup.size(); i++)
            ip::tAddr a = hostGroup[i];
    }
}

void exceptionTest(const tTest& t)
{
    try
    {
        // This will require resolution, so an exception will be thrown.
        ip::tAddrGroup g("rhobota.com", false);
        t.fail();
    }
    catch (ip::eHostNotFoundError& e) { }

    try
    {
        // This domain name doesn't exist (hopefully),
        // so an exception will be thrown.
        ip::tAddrGroup g("yayaya.rhobota.com", true);
        t.fail();
    }
    catch (ip::eHostNotFoundError& e) { }
}

int main(int argc, char* argv[])
{
    tCrashReporter::init();

    tTest("Localhost test", localhostTest);
    tTest("IP string test", ipStringTest);
    tTest("Hostname string test", hostnameStringTest);
    tTest("Exception test", exceptionTest);

    return 0;
}
