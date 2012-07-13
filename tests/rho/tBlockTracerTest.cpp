#include "rho/tBlockTracer.h"
#include "rho/tCrashReporter.h"
#include "rho/tTest.h"

#include <sstream>
#include <string>


std::ostringstream gStringStream;

std::string gCorrectOutput = "foo1()\n{\n    foo2()\n    {\n        inner-foo2 block\n        {\n        }\n    }\n}\n";


using namespace rho;


void foo2()
{
    tBlockTracer b("foo2()", gStringStream);

    {
        tBlockTracer b("inner-foo2 block", gStringStream);
    }
}

void foo1()
{
    tBlockTracer b("foo1()", gStringStream);
    foo2();
}

void test(const tTest& t)
{
    foo1();
    t.iseq(gStringStream.str(), gCorrectOutput);
}

int main()
{
    tCrashReporter::init();

    tTest("tBlockTracer test", test);

    return 0;
}
