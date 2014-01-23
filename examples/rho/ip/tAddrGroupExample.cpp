#include <rho/ip/tAddrGroup.h>

#include <iostream>


using namespace rho;
using std::cout;
using std::endl;


void printAddrGroup(const ip::tAddrGroup& group)
{
    for (int i = 0; i < group.size(); i++)
    {
        ip::tAddr addr = group[i];

        switch (addr.getVersion())
        {
            case ip::kIPv4:
                cout << "IPv4: ";
                break;
            case ip::kIPv6:
                cout << "IPv6: ";
                break;
        }

        cout << addr.toString() << endl;
    }
}


int main()
{
    try
    {
        ip::tAddrGroup googleIPs("google.com", true);
        printAddrGroup(googleIPs);
        cout << endl;

        ip::tAddrGroup routerIPs("192.168.1.1", false);
        printAddrGroup(routerIPs);
        cout << endl;
    }
    catch (ip::ebIP& e)
    {
        cout << e.reason() << endl;
        e.printStacktrace(cout);
    }

    return 0;
}
