#include "rho/ip/tAddrGroup.h"

#include <iostream>


using namespace rho;
using std::cout;
using std::endl;


int main()
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
    catch (ip::ebIP& e)
    {
        cout << e.reason() << endl;
        e.printStacktrace(cout);
    }

    return 0;
}
