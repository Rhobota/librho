#include <rho/bObjectTracer.h>
#include <rho/tCrashReporter.h>
#include <rho/tTest.h>

#include <sstream>


std::ostringstream gStingStream;

std::string gCorrectOutput = "tFoo()\ntFoo(const tFoo&)\ntFoo()\noperator= (tFoo&, const tFoo&)\n~tFoo()\n~tFoo()\n";


class tFoo : public rho::bObjectTracer
{
    public:

        tFoo()
            : bObjectTracer("tFoo", gStingStream)
        {
        }
};


void foo(tFoo f)
{
    tFoo b;
    b = f;
}

void test(const rho::tTest& t)
{
    tFoo a;
    foo(a);
    t.iseq(gStingStream.str(), gCorrectOutput);
}

int main()
{
    rho::tCrashReporter::init();

    rho::tTest("bObjectTracer test", test);

    return 0;
}
