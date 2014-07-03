#include <rho/algo/tLCG.h>
#include <rho/tTest.h>
#include <rho/tCrashReporter.h>

using namespace rho;
using std::cout;
using std::endl;


void defaultLCG(const tTest& t)
{
    algo::tLCG lcg;
    t.assert(lcg.next() == 1103527590);
    t.assert(lcg.next() == 377401575);
    t.assert(lcg.next() == 662824084);
    t.assert(lcg.next() == 1147902781);
    t.assert(lcg.next() == 2035015474);
    t.assert(lcg.next() == 368800899);
    t.assert(lcg.next() == 1508029952);
    t.assert(lcg.randMax() == 2147483647);
}


int main()
{
    tCrashReporter::init();

    tTest("Default LCG test", defaultLCG, 1000);

    return 0;
}
