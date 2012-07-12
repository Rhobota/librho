#include "rho/ip/ebIP.h"
#include "rho/tCrashReporter.h"
#include "rho/tTest.h"


using namespace rho;


void test(const tTest& t)
{
    ip::ebIP e1("bla...");
    ip::eSocketCreationError e2("bla...");
    ip::eSocketBindError e3("bla...");
    ip::eHostNotFoundError e4("bla...");
    ip::eHostUnreachableError e5("bla...");

    ebObject* p1 = &e1;
    ip::ebIP* p2 = &e2;
    ip::ebIP* p3 = &e3;
    ip::ebIP* p4 = &e4;
    ip::ebIP* p5 = &e5;
}


int main()
{
    tCrashReporter::init();

    tTest("ebIP test", test);

    return 0;
}
