#ifndef __rho_tTest_h__
#define __rho_tTest_h__


#include <rho/ppcheck.h>
#include <rho/bNonCopyable.h>
#include <rho/ebObject.h>
#include <rho/types.h>

#include <cmath>
#include <iostream>
#include <sstream>
#include <vector>

#ifdef assert
#undef assert
#endif


namespace rho
{


/**
 * Use only in tests stored in the 'tests/' directory! Also, this
 * class' implementation should be completely in this .h file so
 * that it can be compiled into each test without having to link.
 */
class tTest : public bNonCopyable
{
    public:

        typedef void (*testfunc)(const tTest& t);

        tTest(std::string name, testfunc f);

        tTest(std::string name, testfunc f, i32 numIterations);

        std::string name() const;

        template <class A, class B>
        void iseq(const A& a, const B& b) const;

        void iseq(double a, double b,
                  double epsilon = kEpsilon) const;

        void iseq(const std::vector<double>& a,
                  const std::vector<double>& b,
                  double epsilon = kEpsilon) const;

        template <class A, class B>
        void noteq(const A& a, const B& b) const;

        void noteq(double a, double b,
                   double epsilon = kEpsilon) const;

        void noteq(const std::vector<double>& a,
                   const std::vector<double>& b,
                   double epsilon = kEpsilon) const;

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

        static const double kEpsilon;
};


const double tTest::kEpsilon = 1e-10;


tTest::tTest(std::string name, testfunc f)
    : m_name(name)
{
    f(*this);
    std::cout << "Test passed: " << m_name << std::endl;
}

tTest::tTest(std::string name, testfunc f, i32 numIterations)
    : m_name(name)
{
    for (i32 i = 0; i < numIterations; i++)
    {
        f(*this);
    }
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

void tTest::iseq(double a, double b,
                 double epsilon) const
{
    if (std::fabs(a-b) < epsilon)
        return;
    else
        fail();
}

void tTest::iseq(const std::vector<double>& a,
                 const std::vector<double>& b,
                 double epsilon) const
{
    if (a.size() != b.size())
        fail();
    for (size_t i = 0; i < a.size(); i++)
        iseq(a[i], b[i], epsilon);
}

template <class A, class B>
void tTest::noteq(const A& a, const B& b) const
{
    if (a != b)
        return;
    else
        fail();
}

void tTest::noteq(double a, double b,
                  double epsilon) const
{
    if (std::fabs(a-b) >= epsilon)
        return;
    else
        fail();
}

void tTest::noteq(const std::vector<double>& a,
                  const std::vector<double>& b,
                  double epsilon) const
{
    if (a.size() != b.size())
        return;
    for (size_t i = 0; i < a.size(); i++)
        noteq(a[i], b[i], epsilon);
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
