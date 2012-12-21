#include <rho/iOutputStream.h>
#include <rho/tCrashReporter.h>
#include <rho/tTest.h>


using namespace rho;


int gItWorked = 0;


class tFoo : public iOutputStream
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
    iOutputStream* p = new tFoo();
    p->write(NULL, 29);     // Makes sure the method is virtual.
    p->writeAll(NULL, 29);     // Makes sure the method is virtual.
    t.assert(gItWorked == 2);
    delete p;
}


void bufferedOutputStreamTest(const tTest& t)
{
    iOutputStream* stream1 = new tFoo();
    iOutputStream* stream2 = new tBufferedOutputStream(stream1);
    delete stream2;
    delete stream1;
}


int main()
{
    tCrashReporter::init();

    tTest("iOutputStream test", interfaceTest);
    tTest("tBufferedOutputStream test", bufferedOutputStreamTest);

    return 0;
}
