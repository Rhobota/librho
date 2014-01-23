#include <rho/ip/tcp/tSocket.h>

#include <cstdio>
#include <iostream>


using namespace rho;
using std::cout;
using std::endl;


int main()
{
    try
    {
        ip::tAddrGroup addrGroup(ip::tAddrGroup::kLocalhostConnect);
        ip::tcp::tSocket sock(addrGroup, 8080);

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
