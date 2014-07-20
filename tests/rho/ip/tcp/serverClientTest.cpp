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

using namespace rho;
using std::cout;
using std::endl;
using std::vector;


const int kMaxIters = 100;
const int kMaxMessageLen = 1000000;
const int kTestIterations = 2;


static
vector<u8> s_genMessage(algo::iLCG& lcg)
{
    u32 len = (lcg.next() % kMaxMessageLen) + 1;
    vector<u8> mess(len);
    for (u32 i = 0; i < len; i++)
        mess[i] = (lcg.next() % 256);
    return mess;
}


class tServerRunnable : public sync::iRunnable
{
    public:

        tServerRunnable(const tTest& t, u16 serverBindPort, int seed)
            : m_t(t), m_serverBindPort(serverBindPort), m_seed(seed)
        {
        }

        void run()
        {
            ip::tcp::tServer server(m_serverBindPort);
            refc<ip::tcp::tSocket> socket = server.accept();

            algo::tKnuthLCG lcg(m_seed);
            u32 iters = (lcg.next() % kMaxIters);
            bool serverFirst = ((lcg.next() % 2) == 0);
            vector<u8> correct, rcv, snd;

            if (iters == 0)
                return;

            if (!serverFirst)
            {
                correct = s_genMessage(lcg);
                rcv.resize(correct.size());
                socket->readAll(&rcv[0], (i32)rcv.size());
                m_t.assert(rcv == correct);
                --iters;
            }

            for (u32 i = 0; i < iters; i++)
            {
                snd = s_genMessage(lcg);
                socket->writeAll(&snd[0], (i32)snd.size());

                correct = s_genMessage(lcg);
                rcv.resize(correct.size());
                socket->readAll(&rcv[0], (i32)rcv.size());
                m_t.assert(rcv == correct);
            }

            if (!serverFirst)
            {
                snd = s_genMessage(lcg);
                socket->writeAll(&snd[0], (i32)snd.size());
            }
        }

    private:

        const tTest& m_t;
        u16 m_serverBindPort;
        int m_seed;
};


class tClientRunnable : public sync::iRunnable
{
    public:

        tClientRunnable(const tTest& t, u16 serverBindPort, int seed)
            : m_t(t), m_serverBindPort(serverBindPort), m_seed(seed)
        {
        }

        void run()
        {
            sync::tThread::msleep(10);
            ip::tAddrGroup addrGroup(ip::tAddrGroup::kLocalhostConnect);
            refc<ip::tcp::tSocket> socket(new ip::tcp::tSocket(addrGroup, m_serverBindPort));

            algo::tKnuthLCG lcg(m_seed);
            u32 iters = (lcg.next() % kMaxIters);
            bool serverFirst = ((lcg.next() % 2) == 0);
            vector<u8> correct, rcv, snd;

            if (iters == 0)
                return;

            if (serverFirst)
            {
                correct = s_genMessage(lcg);
                rcv.resize(correct.size());
                socket->readAll(&rcv[0], (i32)rcv.size());
                m_t.assert(rcv == correct);
                --iters;
            }

            for (u32 i = 0; i < iters; i++)
            {
                snd = s_genMessage(lcg);
                socket->writeAll(&snd[0], (i32)snd.size());

                correct = s_genMessage(lcg);
                rcv.resize(correct.size());
                socket->readAll(&rcv[0], (i32)rcv.size());
                m_t.assert(rcv == correct);
            }

            if (serverFirst)
            {
                snd = s_genMessage(lcg);
                socket->writeAll(&snd[0], (i32)snd.size());
            }
        }

    private:

        const tTest& m_t;
        u16 m_serverBindPort;
        int m_seed;
};


void test(const tTest& t)
{
    static u16 sServerBindPort = 15001;
    ++sServerBindPort;
    int seed = rand();

    sync::tThread serverThread(refc<sync::iRunnable>(new tServerRunnable(t,sServerBindPort,seed)));
    sync::tThread clientThread(refc<sync::iRunnable>(new tClientRunnable(t,sServerBindPort,seed)));

    serverThread.join();
    clientThread.join();
}


int main()
{
    tCrashReporter::init();
    srand((u32)time(0));

    tTest("Server/client test", test, kTestIterations);

    return 0;
}
