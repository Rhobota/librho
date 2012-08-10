#include <rho/sync/ebSync.h>
#include <rho/tTest.h>


using namespace rho;


/*
 * This is just a compilation test.
 */


void test(const tTest& t)
{
    sync::ebSync e1("bla...");
    sync::eThreadCreationError e2;
    sync::eThreadCannotBeJoinedError e3("bla...");
    sync::eThreadCannotBeDetachedError e4("bla...");
    sync::eMutexCreationError e5;
    sync::eDeadlockDetected e6;

    ebObject* p1 = &e1;

    sync::ebSync* p2 = &e2;
    sync::ebSync* p3 = &e3;
    sync::ebSync* p4 = &e4;
    sync::ebSync* p5 = &e5;
    sync::ebSync* p6 = &e6;

    p1->what();     //
    p2->what();     //
    p3->what();     // These lines are to get rid of the "unsued var" error.
    p4->what();     //
    p5->what();     //
    p6->what();     //
}


int main()
{
    tTest("ebSync test", test);

    return 0;
}
