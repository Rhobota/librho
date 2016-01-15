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

#if __MINGW32__
#include <winbase.h>
#endif

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
                m_t.assert(socket->flush());

                correct = s_genMessage(lcg);
                rcv.resize(correct.size());
                socket->readAll(&rcv[0], (i32)rcv.size());
                m_t.assert(rcv == correct);
            }

            if (!serverFirst)
            {
                snd = s_genMessage(lcg);
                socket->writeAll(&snd[0], (i32)snd.size());
                m_t.assert(socket->flush());
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
                m_t.assert(socket->flush());

                correct = s_genMessage(lcg);
                rcv.resize(correct.size());
                socket->readAll(&rcv[0], (i32)rcv.size());
                m_t.assert(rcv == correct);
            }

            if (serverFirst)
            {
                snd = s_genMessage(lcg);
                socket->writeAll(&snd[0], (i32)snd.size());
                m_t.assert(socket->flush());
            }
        }

    private:

        const tTest& m_t;
        u16 m_serverBindPort;
        int m_seed;
};


void dataTest(const tTest& t)
{
    static u16 sServerBindPort = 15001;
    ++sServerBindPort;
    int seed = rand();

    sync::tThread serverThread(refc<sync::iRunnable>(new tServerRunnable(t,sServerBindPort,seed)));
    sync::tThread clientThread(refc<sync::iRunnable>(new tClientRunnable(t,sServerBindPort,seed)));

    serverThread.join();
    clientThread.join();
}


class tLittleServerRunnable : public sync::iRunnable
{
    public:

        tLittleServerRunnable(const tTest& t, u16 serverBindPort)
            : m_t(t), m_serverBindPort(serverBindPort)
        {
        }

        void run()
        {
            // This server will listen on IPv6 localhost (aka, "::").
            // But it SHOULD be configured to also accept connections
            // from IPv4 hosts.
            ip::tAddrGroup addrGroup(ip::tAddrGroup::kWildcardBind);
            ip::tcp::tServer server(addrGroup, m_serverBindPort);
            refc<ip::tcp::tSocket> socket = server.accept();
            m_t.assert(socket);
        }

    private:

        const tTest& m_t;
        u16 m_serverBindPort;
};


class tLittleClientRunnable : public sync::iRunnable
{
    public:

        tLittleClientRunnable(const tTest& t, u16 serverBindPort)
            : m_t(t), m_serverBindPort(serverBindPort)
        {
        }

        void run()
        {
            // We will be an IPv4 host by specifying the localhost IPv4 address.
            // If the server accepts IPv4 connection like it is supposed to, all
            // will go well. Otherwise we'll get an exception here.
            sync::tThread::msleep(10);
            refc<ip::tcp::tSocket> socket(new ip::tcp::tSocket("127.0.0.1", m_serverBindPort));
            m_t.assert(socket);
        }

    private:

        const tTest& m_t;
        u16 m_serverBindPort;
};


void v6onlyOffTest(const tTest& t)
{
#if __MINGW32__
    // NOTE: This test fails on Windows XP, because XP doesn't support the IPV6_V6ONLY feature.
    // SEE: http://msdn.microsoft.com/en-us/library/windows/desktop/ms738574(v=vs.85).aspx
    // I don't ever plan to fix this. I don't think it's worth the time since XP is so damn old.
    if (((DWORD)(LOBYTE(LOWORD(GetVersion())))) <= 5)
    {
        cout << "Skipping test because we're running on Windows XP (or earlier). This test would fail." << endl;
        return;
    }
#endif

    static u16 sServerBindPort = 16001;
    ++sServerBindPort;

    sync::tThread serverThread(refc<sync::iRunnable>(new tLittleServerRunnable(t, sServerBindPort)));
    sync::tThread clientThread(refc<sync::iRunnable>(new tLittleClientRunnable(t, sServerBindPort)));

    serverThread.join();
    clientThread.join();
}


int main()
{
    tCrashReporter::init();
    srand((u32)time(0));

    tTest("Server/client data test", dataTest, kTestIterations);
    tTest("Server/client v6only is off test", v6onlyOffTest, kTestIterations);

    return 0;
}
