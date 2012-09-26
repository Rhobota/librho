#include <rho/geo/tMatrix.h>
#include <rho/tCrashReporter.h>
#include <rho/tTest.h>

#include <iostream>
#include <sstream>
#include <string>


using namespace rho;
using std::cout;
using std::endl;


void matrixTest1(const tTest& t)
{
    geo::tMatrix m = geo::tMatrix::identity();
    m[0][0] = 0;
}


int main()
{
    tCrashReporter::init();

    tTest("Matrix test 1", matrixTest1);

    return 0;
}
