#include "tTCPSocket.h"

#include <cstdio>

using namespace rho;


int main(int argc, char* argv[])
{
    inet::tTCPSocket socket("192.168.1.10", 22);

    u8 buf[100];
    int read;
    while ((read = socket.read(buf, 100)) > 0)
    {
        for (int i = 0; i < read; i++)
            printf("%c", buf[i]);
    }

    return 0;
}
