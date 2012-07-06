#include "rho/ip/tcp/tServer.h"
#include "rho/ip/tcp/tSocket.h"
#include "rho/tCrashReporter.h"
#include "rho/tTest.h"

#include <iostream>
#include <string>


/*
 * This is unimplemented. This library has no thread class yes, which I need
 * to do a combined tSocket/tServer test.
 */


using namespace rho;
using std::cout;
using std::endl;


int main()
{
    for (int i = 0; i < 10; i++)
    {
        try
        {
            ip::tcp::tServer server1(8080);
            ip::tcp::tServer server2(8081);
        }
        catch (std::exception& e)
        {
            cout << e.what() << endl;
        }
    }

    return 0;
}
