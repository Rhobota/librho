#include <rho/bObjectTracer.h>
#include <rho/tBlockTracer.h>

#include <iostream>


using namespace rho;


class tFoo : public bObjectTracer  // the bObjectTracer will print in c'tor and d'tor
{
    public:

        tFoo() : bObjectTracer("tFoo") { }
};


void baz(tFoo f)
{
    RHO_BT();   // the tBlockTracer will print in the c'tor and d'tor

    tFoo l;
    l = f;
}


int main()
{
    RHO_BT();   // the tBlockTracer will print in the c'tor and d'tor

    tFoo f;

    baz(f);

    return 0;
}
