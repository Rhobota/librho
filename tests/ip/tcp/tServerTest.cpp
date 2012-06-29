#include "rho/ip/tcp/tServer.h"

#include <iostream>
#include <string>


using namespace rho;
using std::cout;
using std::cin;
using std::endl;


int main()
{
    try
    {
        ip::tcp::tServer server(8080);

        refc<ip::tcp::tSocket> sock = server.accept();

        std::string text = "Hello!\n";
        sock->write((const u8*)text.c_str(), text.length());
    }
    catch (std::exception& e)
    {
        cout << "Error: " << e.what() << endl;
    }

    return 0;
}
