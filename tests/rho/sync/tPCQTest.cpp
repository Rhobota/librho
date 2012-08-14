#include <rho/sync/tPCQ.h>
#include <rho/refc.h>
#include <rho/sync/tThread.h>
#include <rho/tCrashReporter.h>
#include <rho/tTest.h>

using namespace rho;


typedef sync::tPCQ<u32> tIntPCQ;


class tConsumeOne : public sync::iRunnable
{
    public:

        tConsumeOne(u32 msleep, refc<tIntPCQ> pcq, bool* doneflag)
            : m_msleep(msleep),
              m_pcq(pcq),
              m_doneflag(doneflag)
        {
        }

        void run()
        {
            sync::tThread::msleep(m_msleep);
            *m_doneflag = true;
            m_pcq->pop();
        }

    private:

        u32 m_msleep;
        refc<tIntPCQ> m_pcq;
        bool* m_doneflag;
};


void discardTest(const tTest& t)
{
    u32 cap = 10;

    tIntPCQ q(cap, sync::kDiscard);
    t.iseq((u32)0, q.size());
    t.iseq(cap, q.capacity());

    for (u32 i = 0; i < cap; i++)
    {
        q.push(i);
        t.iseq(i+1, q.size());
        t.iseq(cap, q.capacity());
    }

    t.iseq(cap, q.size());
    t.iseq(cap, q.capacity());
    q.push(100);
    t.iseq(cap, q.size());            // didn't change from above...
    t.iseq(cap, q.capacity());        // ...

    for (u32 i = 0; i < cap; i++)
    {
        u32 f = q.pop();
        t.iseq(f, i);
        t.iseq(cap-i-1, q.size());
        t.iseq(cap, q.capacity());
    }

    t.iseq((u32)0, q.size());
    t.iseq(cap, q.capacity());
}


void blockTest(const tTest& t)
{
    u32 cap = 10;

    refc<tIntPCQ> q = new tIntPCQ(cap, sync::kBlock);
    t.iseq((u32)0, q->size());
    t.iseq(cap, q->capacity());

    for (u32 i = 0; i < cap; i++)
    {
        q->push(i);
        t.iseq(i+1, q->size());
        t.iseq(cap, q->capacity());
    }

    t.iseq(cap, q->size());
    t.iseq(cap, q->capacity());

    bool doneflag = false;

    tConsumeOne* co = new tConsumeOne(100, q, &doneflag);
    sync::tThread ct(co);

    q->push(100);

    t.iseq(doneflag, true);

    ct.join();
}


void growTest(const tTest& t)
{
    u32 cap = 10;

    tIntPCQ q(cap, sync::kGrow);
    t.iseq((u32)0, q.size());
    t.iseq(cap, q.capacity());

    for (u32 i = 0; i < cap; i++)
    {
        q.push(i);
        t.iseq(i+1, q.size());
        t.iseq(cap, q.capacity());
    }

    t.iseq(cap, q.size());
    t.iseq(cap, q.capacity());
    q.push(10);
    t.iseq(cap+1, q.size());
    t.iseq(cap*2, q.capacity());

    for (u32 i = 0; i < cap+1; i++)
    {
        u32 f = q.pop();
        t.iseq(f, i);
        t.iseq(cap-i, q.size());
        t.iseq(cap*2, q.capacity());
    }

    t.iseq((u32)0, q.size());
    t.iseq(cap*2, q.capacity());
}


void throwTest(const tTest& t)
{
    u32 cap = 10;

    tIntPCQ q(cap, sync::kThrow);
    t.iseq((u32)0, q.size());
    t.iseq(cap, q.capacity());

    for (u32 i = 0; i < cap; i++)
    {
        q.push(i);
        t.iseq(i+1, q.size());
        t.iseq(cap, q.capacity());
    }

    t.iseq(cap, q.size());
    t.iseq(cap, q.capacity());

    try
    {
        q.push(10);
        t.fail();
    }
    catch (eBufferOverflow& e)
    {
        // Yay!
    }
}


void stressTest(const tTest& t)
{
}


int main()
{
    tCrashReporter::init();

    tTest("PCQ discard test", discardTest);
    tTest("PCQ block test", blockTest);
    tTest("PCQ grow test", growTest);
    tTest("PCQ throw test", throwTest);
    tTest("PCQ stress test", stressTest);

    return 0;
}
