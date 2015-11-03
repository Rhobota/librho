#include <rho/ip/udp/tSocket.h>

#include <iostream>

using namespace std;
using rho::u8;
using rho::u16;
using rho::i32;


int main(int argc, char* argv[])
{
    rho::ip::tAddrGroup multicastAddrGroup("FF02::6713:1234");  // <-- an arbitrary ipv6 link-local multicast address
    rho::ip::tAddr multicastAddr = multicastAddrGroup[0];
    u16 bindPort = 12347;  // <-- an arbitrary port

    rho::ip::udp::tSocket udpSocket(multicastAddr, bindPort);

    while (true)
    {
        const i32 BUFSIZE = 1024;
        u8 buf[BUFSIZE];

        i32 bufUsed;
        u16 port;
        rho::ip::tAddr addr = udpSocket.receive(buf, BUFSIZE, bufUsed, port);

        string str((char*)buf, std::min(BUFSIZE, bufUsed));

        cout << "Got datagram from " << addr.toString() << " port " << port << ": " << str << endl;
    }

    return 0;
}
