#include <rho/ebObject.h>
#include <rho/tCrashReporter.h>
#include <rho/tTest.h>

#include <iostream>
#include <string>


using namespace rho;


class tFooError : public ebObject
{
    public:

        tFooError(std::string reason)
            : ebObject(reason)
        {
        }

        ~tFooError() throw() { }
};


void test(const tTest& t)
{
    std::string reason = "There was a super bad error; oh no!!!";
    try
    {
        throw tFooError(reason);
        t.fail();
    }
    catch (ebObject& e)
    {
        t.iseq(e.reason(), reason);
        t.iseq(std::string(e.what()), reason);

        std::ostringstream o;
        e.printStacktrace(o);   // just a compile test
    }
}

int main()
{
    tCrashReporter::init();

    tTest("ebObject test", test);

    return 0;
}
