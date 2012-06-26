#include "rho/ip/tAddrGroup.h"

#include <iostream>


using namespace rho;
using std::cout;
using std::cin;
using std::endl;


int main(int argc, char* argv[])
{
    try
    {
        ip::tAddrGroup googleIPs("google.com", true);

        for (int i = 0; i < googleIPs.size(); i++)
        {
            cout << googleIPs[i].toString() << endl;
        }

        ip::tAddrGroup routerIPs("192.168.1.1", false);

        for (int i = 0; i < routerIPs.size(); i++)
        {
            cout << routerIPs[i].toString() << endl;
        }
    }
    catch (std::exception& e)
    {
        cout << e.what() << endl;
    }

    return 0;
}
