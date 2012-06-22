#include "tAddr.h"
// #include "tTCPSocket.h"

#include <iostream>

using namespace rho;
using std::cout;
using std::cin;
using std::endl;


int main(int argc, char* argv[])
{
//     inet::tTCPSocket socket("192.168.1.10", 22);
//
//     u8 buf[100];
//     int read;
//     while ((read = socket.read(buf, 100)) > 0)
//     {
//         for (int i = 0; i < read; i++)
//             printf("%c", buf[i]);
//     }

    try
    {
        cout << inet::tAddr("google.com", true).toString() << endl;
        cout << inet::tAddr("192.168.1.1", false).toString() << endl;
    }
    catch (std::exception& e)
    {
        cout << e.what() << endl;
    }

    return 0;
}
