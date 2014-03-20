#include <rho/sync/tPCQ.h>
#include <rho/refc.h>
#include <rho/sync/tThread.h>
#include <rho/tCrashReporter.h>
#include <rho/tTest.h>

#include <cstdlib>
#include <ctime>
#include <iostream>
#include <vector>

using namespace rho;
using std::cout;
using std::endl;
using std::vector;


typedef sync::tPCQ<u32> tIntPCQ;


const int kNumThingsToProduce = 10000;
const int kNumProducers = 2;
const int kNumConsumers = 10;


u32 next(u32 val)
{
    return (val*27+13) % 32;   // stupid lcg, but good enough for this test
}


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


class tConsumer : public sync::iRunnable
{
    public:

        tConsumer(int numThingsToConsume, refc<tIntPCQ> pcq)
            : m_numThingsToConsume(numThingsToConsume),
              m_pcq(pcq)
        {
        }

        void consumeThing(u32 thing)
        {
            // Normally something would be done here...
        }

        void run()
        {
            for (int i = 0; i < m_numThingsToConsume; i++)
            {
                u32 thing = m_pcq->pop();
                consumeThing(thing);
            }
        }

    private:

        int m_numThingsToConsume;
        refc<tIntPCQ> m_pcq;
};


class tProducer : public sync::iRunnable
{
    public:

        tProducer(int numThingsToProduce, refc<tIntPCQ> pcq)
            : m_numThingsToProduce(numThingsToProduce),
              m_pcq(pcq)
        {
        }

        u32 produceThing()
        {
            // Normally this would be more involved.
            return rand() % 10;
        }

        void run()
        {
            for (int i = 0; i < m_numThingsToProduce; i++)
            {
                u32 thing = produceThing();
                m_pcq->push(thing);
            }
        }

    private:

        int m_numThingsToProduce;
        refc<tIntPCQ> m_pcq;
};


class tLCGProducer : public sync::iRunnable
{
    public:

        tLCGProducer(int numThingsToProduce, refc<tIntPCQ> pcq)
            : m_numThingsToProduce(numThingsToProduce),
              m_pcq(pcq),
              m_val(0)
        {
        }

        u32 produceThing()
        {
            m_val = next(m_val);
            return m_val;
        }

        void run()
        {
            for (int i = 0; i < m_numThingsToProduce; i++)
            {
                u32 thing = produceThing();
                m_pcq->push(thing);
            }
        }

    private:

        int m_numThingsToProduce;
        refc<tIntPCQ> m_pcq;
        u32 m_val;
};


class tStatusPrinter : public sync::iRunnable
{
    public:

        tStatusPrinter(refc<tIntPCQ> pcq)
            : m_pcq(pcq)
        {
        }

        void run()
        {
            while (m_pcq.count() > 2)    // super-hack!
            {
                cout << "PCQ size: " << m_pcq->size() << "\n";
            }
        }

    private:

        refc<tIntPCQ> m_pcq;
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

    refc<tIntPCQ> q(new tIntPCQ(cap, sync::kBlock));
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

    refc<sync::iRunnable> co(new tConsumeOne(100, q, &doneflag));
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


void stressTest(const tTest& t, refc<tIntPCQ> pcq)
{
    vector< refc<sync::tThread> > consumerThreads, producerThreads;

    //sync::tThread statusThread(new tStatusPrinter(pcq));

    for (int i = 0; i < kNumConsumers; i++)
    {
        int numThingsToConsume = kNumThingsToProduce / kNumConsumers;
        refc<sync::iRunnable> consumer(new tConsumer(numThingsToConsume, pcq));
        refc<sync::tThread> consumerThread(new sync::tThread(consumer));
        consumerThreads.push_back(consumerThread);
    }

    for (int i = 0; i < kNumProducers; i++)
    {
        int numThingsToProduce = kNumThingsToProduce / kNumProducers;
        refc<sync::iRunnable> producer(new tProducer(numThingsToProduce, pcq));
        refc<sync::tThread> producerThread(new sync::tThread(producer));
        producerThreads.push_back(producerThread);
    }

    for (int i = 0; i < kNumConsumers; i++)
    {
        consumerThreads[i]->join();
    }
    consumerThreads.clear();

    for (int i = 0; i < kNumProducers; i++)
    {
        producerThreads[i]->join();
    }
    producerThreads.clear();

    //statusThread.join();

    t.iseq((u32)0, pcq->size());
}


void stressTest(const tTest& t)
{
    {
        refc<tIntPCQ> pcq(new tIntPCQ(1, sync::kGrow));
        stressTest(t, pcq);
        t.assert((u32)1 < pcq->capacity());
    }

    {
        refc<tIntPCQ> pcq(new tIntPCQ(1, sync::kBlock));
        stressTest(t, pcq);
        t.iseq((u32)1, pcq->capacity());
    }
}


void orderTest(const tTest& t, refc<tIntPCQ> pcq)
{
    sync::tThread producerThread(refc<sync::iRunnable>(new tLCGProducer(kNumThingsToProduce, pcq)));

    u32 val = 0;
    for (int i = 0; i < kNumThingsToProduce; i++)
    {
        val = next(val);
        u32 thing = pcq->pop();
        t.iseq(val, thing);
    }

    producerThread.join();
}


void orderTest(const tTest& t)
{
    {
        refc<tIntPCQ> pcq(new tIntPCQ(1, sync::kGrow));
        orderTest(t, pcq);
        t.assert((u32)1 < pcq->capacity());
    }

    {
        refc<tIntPCQ> pcq(new tIntPCQ(1, sync::kBlock));
        orderTest(t, pcq);
        t.iseq((u32)1, pcq->capacity());
    }
}


int main()
{
    tCrashReporter::init();

    srand((u32)time(0));

    tTest("Discard test", discardTest);
    tTest("Block test", blockTest);
    tTest("Grow test", growTest);
    tTest("Throw test", throwTest);
    tTest("Stress test", stressTest);
    tTest("Order test", orderTest);

    return 0;
}
