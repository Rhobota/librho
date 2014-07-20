#include <rho/iReadable.h>
#include <rho/iWritable.h>
#include <rho/tCrashReporter.h>
#include <rho/tTest.h>

#include <vector>
#include <cstdlib>
#include <ctime>

using namespace rho;
using std::vector;


#if __linux__ || __APPLE__ || __CYGWIN__
std::string gFilePath = "/tmp/randfile.bin";
#elif __MINGW32__
std::string gFilePath = "C:\\randfile.bin";
#else
#error What platform are you on!?
#endif


void test(const tTest& t)
{
    i32 buflen = (rand() % 10000000);
    vector<u8> buf(buflen);
    for (i32 i = 0; i < buflen; i++)
        buf[i] = (rand() % 256);

    {
        tFileWritable file(gFilePath);
        if (buflen > 0)
            file.writeAll(&buf[0], buflen);
    }

    vector<u8> buffromfile;

    {
        tFileReadable file(gFilePath);
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
