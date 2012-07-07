#ifndef __rho_tTest_h__
#define __rho_tTest_h__


#include "ebObject.h"

#include <iostream>
#include <sstream>


namespace rho
{


class tTest
{
    public:

        typedef void (*testfunc)(const tTest& t);

        tTest(std::string name, testfunc f);

        std::string name() const;

        template <class A, class B>
        void iseq(const A& a, const B& b) const;

        template <class A, class B>
        void noteq(const A& a, const B& b) const;

        void assert(bool b) const;

        void reject(bool b) const;

        void fail() const;

    public:

        class eTestFailedError : public ebObject
        {
            public:
                eTestFailedError(std::string reason)
                    : ebObject(reason)
                {
                }

                ~eTestFailedError() throw() { }
        };

    private:

        std::string m_name;
};


tTest::tTest(std::string name, testfunc f)
    : m_name(name)
{
    f(*this);
    std::cout << "Test passed: " << m_name << std::endl;
}

std::string tTest::name() const
{
    return m_name;
}

template <class A, class B>
void tTest::iseq(const A& a, const B& b) const
{
    if (a == b)
        return;
    else
        fail();
}

template <class A, class B>
void tTest::noteq(const A& a, const B& b) const
{
    if (a != b)
        return;
    else
        fail();
}

void tTest::assert(bool b) const
{
    if (b)
        return;
    else
        fail();
}

void tTest::reject(bool b) const
{
    if (!b)
        return;
    else
        fail();
}

void tTest::fail() const
{
    std::ostringstream o;
    o << "Test failed: " << m_name;
    throw eTestFailedError(o.str());
}


}   // namespace rho


#endif   // __rho_tTest_h__
