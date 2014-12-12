#include <rho/sync/tStreamPCQ.h>
#include <rho/sync/tThread.h>
#include <rho/tTest.h>
#include <rho/tCrashReporter.h>
#include <rho/algo/tLCG.h>

#include <cstdlib>
#include <ctime>
#include <iostream>

using namespace rho;
using std::cout;
using std::endl;


static const u64 kMaxTotalLen = 10000000;
static const u64 kMaxReadWrite = 10000;
static const u32 kNumTests = 10;


class tConsumer : public sync::iRunnable
{
    public:

        tConsumer(refc<iReadable> readable, int sharedSeed, const tTest& t)
            : m_readable(readable), m_t(t)
        {
            m_selfSeed = rand();
            m_sharedSeed = sharedSeed;
        }

        void run()
        {
            algo::tKnuthLCG selfLCG(m_selfSeed);
            algo::tKnuthLCG sharedLCG(m_sharedSeed);

            u64 totalLen = sharedLCG.next() % kMaxTotalLen;
            u64 lenSoFar = 0;

            u8* buf = new u8[kMaxReadWrite];

            while (lenSoFar < totalLen)
            {
                u64 lenHere = (selfLCG.next() % kMaxReadWrite) + 1;
                if (lenHere + lenSoFar > totalLen)
                    lenHere = totalLen - lenSoFar;
                u64 r = m_readable->readAll(buf, lenHere);
                m_t.assert(r == lenHere);
                lenSoFar += lenHere;
                for (u64 i = 0; i < r; i++)
                {
                    m_t.assert(buf[i] == (sharedLCG.next() % 256));
                }
            }

            delete [] buf;
            buf = NULL;

            m_t.assert(lenSoFar == totalLen);
        }

    private:

        refc<iReadable> m_readable;
        int m_selfSeed;
        int m_sharedSeed;
        const tTest& m_t;
};


class tProducer : public sync::iRunnable
{
    public:

        tProducer(refc<iWritable> writable, int sharedSeed, const tTest& t)
            : m_writable(writable), m_t(t)
        {
            m_selfSeed = rand();
            m_sharedSeed = sharedSeed;
        }

        void run()
        {
            algo::tKnuthLCG selfLCG(m_selfSeed);
            algo::tKnuthLCG sharedLCG(m_sharedSeed);

            u64 totalLen = sharedLCG.next() % kMaxTotalLen;
            u64 lenSoFar = 0;

            u8* buf = new u8[kMaxReadWrite];

            while (lenSoFar < totalLen)
            {
                u64 lenHere = (selfLCG.next() % kMaxReadWrite) + 1;
                if (lenHere + lenSoFar > totalLen)
                    lenHere = totalLen - lenSoFar;
                for (u64 i = 0; i < lenHere; i++)
                {
                    buf[i] = (sharedLCG.next() % 256);
                }
                u64 w = m_writable->writeAll(buf, lenHere);
                m_t.assert(w == lenHere);
                lenSoFar += lenHere;
            }

            delete [] buf;
            buf = NULL;

            m_t.assert(lenSoFar == totalLen);
        }

    private:

        refc<iWritable> m_writable;
        int m_selfSeed;
        int m_sharedSeed;
        const tTest& m_t;
};


void test(const tTest& t)
{
    refc< sync::tPCQ<std::pair<u8*, u32> > > pcq(new sync::tPCQ<std::pair<u8*, u32> >(8, sync::kGrow));
    refc<iReadable> loopbackReadable(new sync::tStreamPCQ(pcq));
    refc<iWritable> loopbackWritable(new sync::tStreamPCQ(pcq));

    int sharedSeed = rand();

    sync::tThread consumerThread(refc<sync::iRunnable>(new tConsumer(loopbackReadable, sharedSeed, t)));
    sync::tThread producerThread(refc<sync::iRunnable>(new tProducer(loopbackWritable, sharedSeed, t)));

    consumerThread.join();
    producerThread.join();
}


int main()
{
    tCrashReporter::init();

    srand((u32)time(0));

    tTest("tStreamPCQ test", test, kNumTests);

    return 0;
}
