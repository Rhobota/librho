#include <rho/ip/tcp/tServer.h>

#include <iomanip>
#include <iostream>
#include <string>


using namespace rho;
using std::cout;
using std::endl;


int main()
{
    try
    {
        ip::tAddrGroup localhostGroup(ip::tAddrGroup::kWildcardBind);
        ip::tcp::tServer server(localhostGroup, 443);

        cout << "Server bound to address: "
             << server.getBindAddress().toString() << endl;
        cout << "Server bound to port:    " << server.getBindPort() << endl;
        cout << endl;

        cout << "Waiting for connection..." << endl;
        refc<ip::tcp::tSocket> sock = server.accept();
        cout << "Got a connection!" << endl << endl;

        i32 r;
        const i32 BUFSIZE = 1024;
        u8 buf[BUFSIZE];

        while ((r = sock->read(buf, BUFSIZE)) > 0)
        {
            for (i32 i = 0; i < r; i++)
            {
                cout << std::hex << std::setfill('0') << std::setw(2) << ((u32)buf[i]);
                cout << " " << buf[i];
                cout << endl;
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
