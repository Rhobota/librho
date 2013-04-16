#include <rho/algo/tBigInteger.h>
#include <rho/tTest.h>
#include <rho/tCrashReporter.h>

using namespace rho;
using std::cout;
using std::endl;
using std::vector;


void print(algo::tBigInteger bi)
{
    if (bi.isNegative())
        cout << "-";
    vector<u8> bytes = bi.getBytes();
    for (int i = (int)bytes.size()-1; i >= 0; i--)
        cout << " " << (u32)bytes[i];
    cout << endl;
}


bool verrifyEqual32(const algo::tBigInteger& bi, i32 val)
{
    vector<u8> correctBytes;
    u32 uval = val < 0 ? ((u32)(-val)) : ((u32)(val));
    correctBytes.push_back((uval >>  0) & 0xFF);
    correctBytes.push_back((uval >>  8) & 0xFF);
    correctBytes.push_back((uval >> 16) & 0xFF);
    correctBytes.push_back((uval >> 24) & 0xFF);
    while (correctBytes.size() > 0 && correctBytes.back() == 0) correctBytes.pop_back();

    return (bi.isNegative() == (val < 0)) &&
           (bi.getBytes() == correctBytes);
}


bool verrifyEqual64(const algo::tBigInteger& bi, i64 val)
{
    vector<u8> correctBytes;
    u64 uval = val < 0 ? ((u64)(-val)) : ((u64)(val));
    correctBytes.push_back((uval >>  0) & 0xFF);
    correctBytes.push_back((uval >>  8) & 0xFF);
    correctBytes.push_back((uval >> 16) & 0xFF);
    correctBytes.push_back((uval >> 24) & 0xFF);
    correctBytes.push_back((uval >> 32) & 0xFF);
    correctBytes.push_back((uval >> 40) & 0xFF);
    correctBytes.push_back((uval >> 48) & 0xFF);
    correctBytes.push_back((uval >> 56) & 0xFF);
    while (correctBytes.size() > 0 && correctBytes.back() == 0) correctBytes.pop_back();

    return (bi.isNegative() == (val < 0)) &&
           (bi.getBytes() == correctBytes);
}


void i32constructortest(const tTest& t)
{
    vector<i32> v;
    v.push_back(0x7FFFFFFF);  // <-- largest positive int
    v.push_back(0x80000000);  // <-- largest negative int
    v.push_back(0);
    v.push_back(1);
    v.push_back(-1);
    v.push_back(2);
    v.push_back(-2);
    v.push_back(924);
    v.push_back(-520);
    v.push_back(2242924);
    v.push_back(-2284520);
    v.push_back(1182242924);
    v.push_back(-1922284520);

    t.assert(v[0] != -v[0]);    // The largest positive int works as expected.
    t.assert(v[1] == -v[1]);    // The largest negative int is weird, but the
                                // code below relies on this behavior, though
                                // I'm not sure what other behavior could
                                // possibly happen.

    for (size_t i = 0; i < v.size(); i++)
    {
        algo::tBigInteger bi(v[i]);
        t.assert(verrifyEqual32(bi, v[i]));
    }
}


void stringconstructortest(const tTest& t)
{
    // todo
}


void plustest(const tTest& t)
{
    // todo
    // count up from -20000 to 20000 (adds small numbers to big numbers)
    // add big numbers to small numbers
}


void minustest(const tTest& t)
{
    // todo
    // count down from 20000 to -20000 (subtracts small numbers from big numbers)
    // subtract big numbers from small numbers
}


void multtest(const tTest& t)
{
    // todo
}


void divtest(const tTest& t)
{
    // todo
}


int main()
{
    tCrashReporter::init();

    tTest("tBigInteger i32 constructor test", i32constructortest);
    tTest("tBigInteger string constructor test", stringconstructortest);
    tTest("tBigInteger plus test", plustest);
    tTest("tBigInteger minus test", minustest);
    tTest("tBigInteger mult test", multtest);
    tTest("tBigInteger div test", divtest);

    return 0;
}
