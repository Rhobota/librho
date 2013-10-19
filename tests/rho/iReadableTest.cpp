#include <rho/iReadable.h>
#include <rho/tCrashReporter.h>
#include <rho/tTest.h>


using namespace rho;


int gItWorked = 0;


class tFoo : public iReadable
{
    public:

        int read(u8* buffer, int length)
        {
            gItWorked++;
            return 0;
        }

        int readAll(u8* buffer, int length)
        {
            gItWorked++;
            return 0;
        }
};


void interfaceTest(const tTest& t)
{
    iReadable* p = new tFoo();
    p->read(NULL, 78);            // This only tests if the method is virtual...
    p->readAll(NULL, 78);            // This only tests if the method is virtual...
    t.assert(gItWorked == 2);
    delete p;
}


void bufferedReadableTest(const tTest& t)
{
    iReadable* stream1 = new tFoo();
    iReadable* stream2 = new tBufferedReadable(stream1);
    delete stream2;
    delete stream1;
}


int main()
{
    tCrashReporter::init();

    tTest("iReadable test", interfaceTest);
    tTest("tBufferedReadable test", bufferedReadableTest);

    return 0;
}
