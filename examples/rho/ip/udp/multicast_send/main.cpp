#include <rho/ip/udp/tSocket.h>

#include <iostream>

using namespace std;


int main(int argc, char* argv[])
{
    rho::ip::tAddrGroup multicastAddrGroup("FF02::6713:1234");  // <-- an arbitrary ipv6 link-local multicast address
    rho::ip::tAddr multicastAddr = multicastAddrGroup[0];
    rho::u16 port = 12347;  // <-- an arbitrary port

    string message1 = "Hello Ryan! (part 1/2)";
    string message2 = "Hope you're well... (part 2/2)";

    rho::ip::udp::tSocket udpSocket;

    udpSocket.send((rho::u8*) message1.c_str(),
                   (rho::i32) message1.length(),
                   multicastAddr,
                   port);

    udpSocket.send((rho::u8*) message2.c_str(),
                   (rho::i32) message2.length(),
                   multicastAddr,
                   port);

    return 0;
}
