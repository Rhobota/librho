#include <rho/iWritable.h>
#include <rho/tCrashReporter.h>
#include <rho/tTest.h>


using namespace rho;


int gItWorked = 0;


class tFoo : public iWritable
{
    public:

        int write(const u8* buffer, int length)
        {
            gItWorked++;
            return 0;
        }

        int writeAll(const u8* buffer, int length)
        {
            gItWorked++;
            return 0;
        }
};


void interfaceTest(const tTest& t)
{
    iWritable* p = new tFoo();
    p->write(NULL, 29);     // Makes sure the method is virtual.
    p->writeAll(NULL, 29);     // Makes sure the method is virtual.
    t.assert(gItWorked == 2);
    delete p;
}


void bufferedWritableTest(const tTest& t)
{
    iWritable* stream1 = new tFoo();
    iWritable* stream2 = new tBufferedWritable(stream1);
    delete stream2;
    delete stream1;
}


int main()
{
    tCrashReporter::init();

    tTest("iWritable test", interfaceTest);
    tTest("tBufferedWritable test", bufferedWritableTest);

    return 0;
}
