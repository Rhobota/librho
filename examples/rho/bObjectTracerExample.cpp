#include "rho/bObjectTracer.h"

#include <iostream>


using namespace rho;


class tFoo : public bObjectTracer
{
    public:

        tFoo() : bObjectTracer("tFoo") { }
};


void baz(tFoo f)
{
    RHO_BT();

    tFoo l;
    l = f;
}


int main()
{
    RHO_BT();

    tFoo f;

    baz(f);

    return 0;
}
