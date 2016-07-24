#include <rho/eRho.h>
#include <rho/tTest.h>
#include <rho/tCrashReporter.h>

using namespace rho;


void test(const tTest& t)
{
    // This test really isn't worth writing.
}


int main()
{
    tCrashReporter::init();

    tTest("eRho test", test);

    return 0;
}
