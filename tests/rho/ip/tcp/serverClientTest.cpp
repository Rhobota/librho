#include <rho/ip/tcp/tServer.h>
#include <rho/ip/tcp/tSocket.h>
#include <rho/sync/tThread.h>
#include <rho/tCrashReporter.h>
#include <rho/tTest.h>
#include <rho/types.h>

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


const int kTestIterations = 20;
const int kMaxSendRecvIterations = 50;

u16 gServerBindPort = 15001;

vector<u8> gServerWriteData;
vector<u8> gClientWriteData;

int gSendRecvIterations = 0;


void randomizeWriteData(vector<u8>& data)
{
    int writeDataSize = rand() % 100000 + 1;
    data = vector<u8>(writeDataSize);
    for (int i = 0; i < writeDataSize; i++)
        data[i] = (u8) (rand() % 256);
}


void writeHelper(ip::tcp::tSocket& socket, const vector<u8>& data)
{
    int length = data.size();
    u8* buff = new u8[length];
    for (int i = 0; i < length; i++)
        buff[i] = data[i];
    int currLen = 0;
    while (length > currLen)
    {
        currLen += socket.write(buff+currLen, length-currLen);
    }
    delete buff;
}


vector<u8> readHelper(ip::tcp::tSocket& socket, int length)
{
    vector<u8> data;
    u8 buff[1024];
    int currLen = 0;
    while (length > currLen)
    {
        int r = socket.read(buff, std::min(1024, length-currLen));
        for (int i = 0; i < r; i++)
            data.push_back(buff[i]);
        currLen += r;
    }
    return data;
}


class tServerRunnable : public sync::iRunnable
{
    public:

        tServerRunnable(const tTest& t) : m_t(t) { }

        void run()
        {
            ip::tcp::tServer server(gServerBindPort);
            refc<ip::tcp::tSocket> socket = server.accept();

            bool serverFirst = gServerWriteData[0] > gClientWriteData[0];

            int iterations = gSendRecvIterations;

            if (serverFirst)
            {
                writeHelper(*socket, gServerWriteData);
                iterations--;
            }

            for (int i = 0; i < iterations; i++)
            {
                vector<u8> read = readHelper(*socket, gClientWriteData.size());
                m_t.iseq(read, gClientWriteData);
                randomizeWriteData(gClientWriteData);
                writeHelper(*socket, gServerWriteData);
            }

            if (serverFirst)
            {
                vector<u8> read = readHelper(*socket, gClientWriteData.size());
                m_t.iseq(read, gClientWriteData);
            }
        }

    private:

        const tTest& m_t;
};


class tClientRunnable : public sync::iRunnable
{
    public:

        tClientRunnable(const tTest& t) : m_t(t) { }

        void run()
        {
            sync::tThread::msleep(10);

            ip::tAddrGroup addrGroup(ip::tAddrGroup::kLocalhostConnect);
            ip::tcp::tSocket socket(addrGroup, gServerBindPort);

            bool serverFirst = gServerWriteData[0] > gClientWriteData[0];

            int iterations = gSendRecvIterations;

            if (!serverFirst)
            {
                writeHelper(socket, gClientWriteData);
                iterations--;
            }

            for (int i = 0; i < iterations; i++)
            {
                vector<u8> read = readHelper(socket, gServerWriteData.size());
                m_t.iseq(read, gServerWriteData);
                randomizeWriteData(gServerWriteData);
                writeHelper(socket, gClientWriteData);
            }

            if (!serverFirst)
            {
                vector<u8> read = readHelper(socket, gServerWriteData.size());
                m_t.iseq(read, gServerWriteData);
            }
        }

    private:

        const tTest& m_t;
};


void test(const tTest& t)
{
    ++gServerBindPort;
    randomizeWriteData(gServerWriteData);
    randomizeWriteData(gClientWriteData);
    gSendRecvIterations = rand() % kMaxSendRecvIterations + 1;

    sync::tThread serverThread(refc<sync::iRunnable>(new tServerRunnable(t)));
    sync::tThread clientThread(refc<sync::iRunnable>(new tClientRunnable(t)));

    serverThread.join();
    clientThread.join();
}


int main()
{
    tCrashReporter::init();

    srand(time(0));
    tTest("Server/client test", test, kTestIterations);

    return 0;
}
