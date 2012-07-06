#include "rho/ip/tAddr.h"
#include "rho/ip/tAddrGroup.h"
#include "rho/tCrashReporter.h"
#include "rho/tTest.h"

#include <iostream>
#include <string>
#include <vector>


/*
 * Note: tAddr isn't constructable by client code, so to test it we'll have
 * to use tAddrGroup to get some tAddr objects.
 */


using namespace rho;
using std::cout;
using std::endl;


void ipv4Test(const tTest& t)
{
    std::string ipStr = "2.3.4.5";
    std::vector<u8> correctIpBytes;
    correctIpBytes.push_back(2);
    correctIpBytes.push_back(3);
    correctIpBytes.push_back(4);
    correctIpBytes.push_back(5);

    ip::tAddrGroup g(ipStr, false);
    t.iseq(g.size(), 1);

    ip::tAddr addr = g[0];
    t.iseq(addr.getVersion(), ip::kIPv4);
    t.iseq(addr.toString(), ipStr);
    t.iseq(addr.getAddress(), correctIpBytes);
}

void ipv6Test(const tTest& t)
{
    std::string ipStr = "1122:3344:5566:7788:99aa:bbcc:ddee:11ff";
    std::vector<u8> correctIpBytes;
    correctIpBytes.push_back(0x11);
    correctIpBytes.push_back(0x22);
    correctIpBytes.push_back(0x33);
    correctIpBytes.push_back(0x44);
    correctIpBytes.push_back(0x55);
    correctIpBytes.push_back(0x66);
    correctIpBytes.push_back(0x77);
    correctIpBytes.push_back(0x88);
    correctIpBytes.push_back(0x99);
    correctIpBytes.push_back(0xAA);
    correctIpBytes.push_back(0xBB);
    correctIpBytes.push_back(0xCC);
    correctIpBytes.push_back(0xDD);
    correctIpBytes.push_back(0xEE);
    correctIpBytes.push_back(0x11);
    correctIpBytes.push_back(0xFF);

    ip::tAddrGroup g(ipStr, false);
    t.iseq(g.size(), 1);

    ip::tAddr addr = g[0];
    t.iseq(addr.getVersion(), ip::kIPv6);
    t.iseq(addr.toString(), ipStr);
    t.iseq(addr.getAddress(), correctIpBytes);
}

int main()
{
    tCrashReporter::init();

    tTest("ipv4 test", ipv4Test);
    tTest("ipv6 test", ipv6Test);

    return 0;
}
