#include <rho/ip/tcp/tSocket.h>

#include <cstdio>
#include <iostream>


using namespace rho;
using std::cout;
using std::endl;


int main(int argc, char* argv[])
{
    std::string addrStr = (argc > 1) ? argv[1] : "rhobox.com";
    std::string portStr = (argc > 2) ? argv[2] : "22";

    try
    {
        ip::tAddrGroup addrGroup(addrStr);
        ip::tcp::tSocket sock(addrGroup, atoi(portStr.c_str()), 1000);  // <-- 1 second timeout

        ip::tAddr addr = sock.getForeignAddress();
        u16       port = sock.getForeignPort();

        cout << "Foreign address: " << addr.toString() << endl;
        cout << "Foreign port:    " << port << endl;

        ip::tAddr addrLocal = sock.getLocalAddress();
        u16       portLocal = sock.getLocalPort();

        cout << "Local address: " << addrLocal.toString() << endl;
        cout << "Local port:    " << portLocal << endl;

        u8  buf[100];
        int c;
        while ((c = sock.read(buf, 100)) > 0)
        {
            for (int i = 0; i < c; i++)
            {
                printf("%c", buf[i]);
            }
        }
    }
    catch (ip::ebIP& e)
    {
        cout << "Error: " << e.reason() << endl;
        e.printStacktrace(cout);
    }

    return 0;
}
