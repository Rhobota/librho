#include <rho/tBlockTracer.h>

#include <iostream>


using namespace rho;


void bar()
{
    RHO_BT();

    int a = 47;
    int b = 81;

    RHO_VAR(a);
    RHO_VAR(b);
}


void foo(int y)
{
    RHO_BT();

    RHO_VAR(y);

    bar();
}


int main()
{
    RHO_LOG("Demonstrating the RHO_BT(), RHO_VAR(), and RHO_LOG() macros...");

    RHO_BT();

    foo(27);

    return 0;
}
