#include <rho/ip/tcp/tServer.h>
#include <rho/ip/tcp/tSocket.h>
#include <rho/sync/tThread.h>
#include <rho/tCrashReporter.h>
#include <rho/tTest.h>
#include <rho/types.h>
#include <rho/algo/tLCG.h>

#include <cstdlib>
#include <cmath>
#include <ctime>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>


using namespace rho;
using std::cout;
using std::endl;
using std::vector;


u16 gServerBindPort = 15001;


class tServerRunnable : public sync::iRunnable
{
    public:

        tServerRunnable(const tTest& t) : m_t(t) { }

        void run()
        {
            ip::tcp::tServer server(
                    ip::tAddrGroup::kWildcardBind, gServerBindPort);

            while (true)
            {
                refc<ip::tcp::tSocket> socket = server.accept();

                std::ofstream outfile("written.txt");

                algo::tKnuthLCG lcg(2492);

                i32 numBytes = 0;
                while (true)
                {
                    u8 buf[1024];
                    lcg.read(buf, 1024);
                    i32 w = socket->writeAll(buf, 1024);
                    for (i32 i = 0; i < w; i++)
                        outfile << (u32)buf[i] << endl;
                    if (w > 0)
                        numBytes += w;
                    if (w != 1024)
                        break;
                }

                cout << "Server socket stopped, wrote " << numBytes << " bytes." << endl;
                outfile.close();
            }
        }

    private:

        const tTest& m_t;
};


void test(const tTest& t)
{
    refc<tServerRunnable> serverRunnable(new tServerRunnable(t));
    refc<sync::iRunnable> sRunnable(serverRunnable);
    sync::tThread serverThread(sRunnable);
    serverThread.join();
}


int main()
{
    tCrashReporter::init();

    tTest("Server/client test", test);

    return 0;
}
