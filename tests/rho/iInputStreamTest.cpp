#include <rho/iInputStream.h>
#include <rho/tCrashReporter.h>
#include <rho/tTest.h>


using namespace rho;


int gItWorked = 0;


class tFoo : public iInputStream
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
    iInputStream* p = new tFoo();
    p->read(NULL, 78);            // This only tests if the method is virtual...
    p->readAll(NULL, 78);            // This only tests if the method is virtual...
    t.assert(gItWorked == 2);
    delete p;
}


void bufferedInputStreamTest(const tTest& t)
{
    iInputStream* stream1 = new tFoo();
    iInputStream* stream2 = new tBufferedInputStream(stream1);
    delete stream2;
    delete stream1;
}


int main()
{
    tCrashReporter::init();

    tTest("iInputStream test", interfaceTest);
    tTest("tBufferedInputStream test", bufferedInputStreamTest);

    return 0;
}
