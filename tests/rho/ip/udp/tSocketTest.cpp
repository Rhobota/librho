#include <rho/ip/udp/tSocket.h>
#include <rho/tCrashReporter.h>
#include <rho/tTest.h>
#include <rho/sync/tTimer.h>
#include <rho/iPackable.h>

#include <iostream>
#include <string>
#include <vector>


using namespace rho;
using std::cerr;
using std::cout;
using std::endl;
using std::string;
using std::vector;


void sendReceiveExternalText(const tTest& t)
{
    unsigned char msg[48] = { 010, 0 };  // <-- NTP query
    ip::udp::tSocket socket;
    socket.send(msg, 48, "0.pool.ntp.org", 123);

    const i32 BUFSIZE = 1024;
    u8 buf[BUFSIZE];
    i32 bufSize;
    u16 port;
    ip::tAddr addr = socket.receive(buf, BUFSIZE, bufSize, port, 5000);
    t.assert(port == 123);

    vector<u8> vtr(buf, buf + std::min(BUFSIZE, bufSize));
    tByteReadable readable(vtr);
    try
    {
        while (true)
        {
            u32 val; rho::unpack(&readable, val);
            //cout << val << endl;
        }
    }
    catch (ebObject& e)
    {
    }
}

void selfLoopUnicastTest(const tTest& t)
{
    // TODO
}

void selfLoopMulticastTest(const tTest& t)
{
    // TODO
}

void timeoutTest(const tTest& t, u32 correctTimeoutMS, u32 allowance)
{
    bool didThrow = false;
    u64 starttime = sync::tTimer::usecTime();
    try
    {
        ip::udp::tSocket socket(12349);

        const i32 BUFSIZE = 1024;
        u8 buf[BUFSIZE];
        i32 bufSize;
        u16 port;
        ip::tAddr addr = socket.receive(buf, BUFSIZE, bufSize, port, correctTimeoutMS);

        t.fail();
    }
    catch (ip::eReceiveTimeoutError& e)
    {
        u64 endtime = sync::tTimer::usecTime();
        u64 elapsed = endtime - starttime;
        u64 elapsedMS = elapsed / 1000;

        if (elapsedMS > correctTimeoutMS+allowance || elapsedMS < correctTimeoutMS-allowance)
        {
            cerr << "elapsedMS: " << elapsedMS << endl;
            cerr << "expectedMS: " << correctTimeoutMS << endl;
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

    tTest("send/receive external test", sendReceiveExternalText);
    tTest("self loop unicast test", selfLoopUnicastTest);
    tTest("self loop multicast test", selfLoopMulticastTest);
    tTest("timeout test", timeoutTest);

    return 0;
}
