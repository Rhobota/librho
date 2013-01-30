#include <rho/geo/tTrans4.h>
#include <rho/tCrashReporter.h>
#include <rho/tTest.h>

#include <iostream>
#include <sstream>
#include <string>


using namespace rho;
using std::cout;
using std::endl;


/**
 * This provides only a compilation test... and not even that very well.
 */


void trans4Test1(const tTest& t)
{
    geo::tTrans4 m = geo::tTrans4::identity();
    m[0][0] = 0;
}


int main()
{
    tCrashReporter::init();

    tTest("tTrans4 test 1", trans4Test1);

    return 0;
}
