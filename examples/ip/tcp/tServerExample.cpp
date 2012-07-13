#include "rho/ip/tcp/tServer.h"

#include <iostream>
#include <string>


using namespace rho;
using std::cout;
using std::endl;


int main()
{
    try
    {
        ip::tAddrGroup localhostGroup(ip::tAddrGroup::kLocalhostBind);
        ip::tcp::tServer server(localhostGroup, 8080);

        cout << "Server bound to address: "
             << server.getBindAddress().toString() << endl;
        cout << "Server bound to port:    " << server.getBindPort() << endl;
        cout << endl;

        cout << "Waiting for connection..." << endl;
        refc<ip::tcp::tSocket> sock = server.accept();
        cout << "Got a connection, will write 'Hello' and exit." << endl;

        std::string text = "Hello!\n";
        sock->write((const u8*)text.c_str(), text.length());
    }
    catch (ip::ebIP& e)
    {
        cout << "Error: " << e.reason() << endl;
        e.printStacktrace(cout);
    }

    return 0;
}
