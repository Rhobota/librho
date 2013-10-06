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

const int kTestIterations = 100000;


class tClientRunnable : public sync::iRunnable
{
    public:

        tClientRunnable(const tTest& t)
            : m_t(t), m_socket(NULL)
        { }

        void run()
        {
            sync::tThread::msleep(10);

            ip::tAddrGroup addrGroup("10.1.1.11");
            refc<ip::tcp::tSocket> socket(new ip::tcp::tSocket(addrGroup, gServerBindPort));
            m_socket = socket;

            algo::tKnuthLCG lcg(2492);

            std::ofstream outfile("read.txt");

            u8 buf[1024];

            i32 numBytes = 0;

            while (true)
            {
                i32 r = socket->readAll(buf, 1024);
                for (i32 i = 0; i < r; i++)
                {
                    outfile << (u32)buf[i] << endl;
                    u8 val; lcg.read(&val, 1);
                    m_t.assert(val == buf[i]);
                }
                if (r > 0)
                    numBytes += r;
                if (r != 1024)
                    break;
            }

            cout << "Read " << numBytes << " bytes" << endl;
            outfile.close();
        }

        void kill()
        {
            while (!m_socket) { }
            m_socket->close();
        }

    private:

        const tTest& m_t;
        refc<ip::tcp::tSocket> m_socket;
};


void test(const tTest& t)
{
    refc<tClientRunnable> clientRunnable(new tClientRunnable(t));
    refc<sync::iRunnable> cRunnable(clientRunnable);
    sync::tThread clientThread(cRunnable);

    sync::tThread::msleep(1000);
    clientRunnable->kill();

    clientThread.join();
}


int main()
{
    tCrashReporter::init();

    tTest("Server/client test", test, kTestIterations);

    return 0;
}
