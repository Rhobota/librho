#include <rho/types.h>
#include <rho/tTest.h>
#include <rho/tCrashReporter.h>

using namespace rho;


class iBaseOne
{
    public:

        virtual void foo() = 0;
};

class iBaseTwo
{
    public:

        virtual void bar() = 0;
};

class tChild : public iBaseOne, public iBaseTwo
{
    public:

        void foo() { }
        void bar() { }
};

class tOther
{
    public:

        virtual void doSomething() { }
};


void rttiTest(const tTest& t)
{
    iBaseOne* one = new tChild();
    iBaseTwo* two = new tChild();
    tChild* child = new tChild();
    tOther* other = new tOther();

    t.assert(dynamic_cast<iBaseOne*>(one) != NULL);
    t.assert(dynamic_cast<iBaseTwo*>(one) != NULL);
    t.assert(dynamic_cast<tChild*>(one) != NULL);
    t.assert(dynamic_cast<tOther*>(one) == NULL);

    t.assert(dynamic_cast<iBaseOne*>(two) != NULL);
    t.assert(dynamic_cast<iBaseTwo*>(two) != NULL);
    t.assert(dynamic_cast<tChild*>(two) != NULL);
    t.assert(dynamic_cast<tOther*>(two) == NULL);

    t.assert(dynamic_cast<iBaseOne*>(child) != NULL);
    t.assert(dynamic_cast<iBaseTwo*>(child) != NULL);
    t.assert(dynamic_cast<tChild*>(child) != NULL);
    t.assert(dynamic_cast<tOther*>(child) == NULL);

    t.assert(dynamic_cast<iBaseOne*>(other) == NULL);
    t.assert(dynamic_cast<iBaseTwo*>(other) == NULL);
    t.assert(dynamic_cast<tChild*>(other) == NULL);
    t.assert(dynamic_cast<tOther*>(other) != NULL);
}


void typesTest(const tTest& t)
{
    t.assert(sizeof(u8) == 1);
    t.assert(sizeof(i8) == 1);
    t.assert(sizeof(u16) == 2);
    t.assert(sizeof(i16) == 2);
    t.assert(sizeof(u32) == 4);
    t.assert(sizeof(i32) == 4);
    t.assert(sizeof(u64) == 8);
    t.assert(sizeof(i64) == 8);

    u8  a;
    u16 b;
    u32 c;
    u64 d;

    i8  e;
    i16 f;
    i32 g;
    i64 h;

    t.assert(sizeof(a) == 1);
    t.assert(sizeof(b) == 2);
    t.assert(sizeof(c) == 4);
    t.assert(sizeof(d) == 8);
    t.assert(sizeof(e) == 1);
    t.assert(sizeof(f) == 2);
    t.assert(sizeof(g) == 4);
    t.assert(sizeof(h) == 8);
}


int main()
{
    tCrashReporter::init();

    tTest("rtti test", rttiTest);
    tTest("types test", typesTest);

    return 0;
}
