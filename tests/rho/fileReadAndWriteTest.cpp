#include <rho/iReadable.h>
#include <rho/iWritable.h>
#include <rho/tCrashReporter.h>
#include <rho/tTest.h>

#include <vector>
#include <cstdlib>
#include <ctime>

using namespace rho;
using std::vector;


void test(const tTest& t)
{
    i32 buflen = (rand() % 10000000);
    vector<u8> buf(buflen);
    for (i32 i = 0; i < buflen; i++)
        buf[i] = (rand() % 256);

    std::string filename = "/tmp/randfile.bin";

    {
        tFileWritable file(filename);
        if (buflen > 0)
            file.writeAll(&buf[0], buflen);
    }

    vector<u8> buffromfile;

    {
        tFileReadable file(filename);
        u8 part[1000];
        i32 r;
        while ((r = file.read(part, 1000)) > 0)
            for (i32 i = 0; i < r; i++)
                buffromfile.push_back(part[i]);
    }

    t.assert(buf == buffromfile);
}


int main()
{
    tCrashReporter::init();
    srand(time(0));

    tTest("file read/write test", test);

    return 0;
}
