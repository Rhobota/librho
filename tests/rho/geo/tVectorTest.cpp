#include <rho/geo/tVector.h>
#include <rho/tCrashReporter.h>
#include <rho/tTest.h>

#include <iostream>
#include <sstream>
#include <string>


using namespace rho;
using std::cout;
using std::endl;


void vectorTest1(const tTest& t)
{
    geo::tVector v(0.0, 0.0);
    v.x = 1;
}


int main()
{
    tCrashReporter::init();

    tTest("Vector test 1", vectorTest1);

    return 0;
}
