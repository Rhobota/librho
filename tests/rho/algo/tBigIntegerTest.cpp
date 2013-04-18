#include <rho/algo/tBigInteger.h>
#include <rho/tTest.h>
#include <rho/tCrashReporter.h>

using namespace rho;
using std::cout;
using std::endl;
using std::vector;


static const int kRandIters = 1000000;


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


i32 bigRand()
{
    i32 val = 0;
    val |= rand() % 256;
    val <<= 8;
    val |= rand() % 256;
    val <<= 8;
    val |= rand() % 256;
    val <<= 8;
    val |= rand() % 256;
    return val;
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


void tostringtest(const tTest& t)
{
    // todo
}


void plustest(const tTest& t)
{
    {
        i32 val = -9000;
        algo::tBigInteger bi(val);
        while (val < 9000)
        {
            t.assert(verrifyEqual32(bi, val));
            val++;
            bi += 1;
        }
    }

    {
        i32 val = 9000;
        algo::tBigInteger bi(val);
        while (val > -9000)
        {
            t.assert(verrifyEqual32(bi, val));
            val--;
            bi += -1;
        }
    }

    for (int i = 0; i < kRandIters; i++)
    {
        i32 val = (rand() % 9000) - 4500;
        algo::tBigInteger bi(val);
        t.assert(verrifyEqual32(bi, val));

        i32 val2 = bigRand();               // <-- could be pos or neg any value
        if ((val > 0 && val2 > 0 && val + val2 <= 0) ||
            (val < 0 && val2 < 0 && val + val2 >= 0))
        {
            // Overflow happened, so we cannot do any testing
            // on this iteration.
            continue;
        }

        val += val2;
        bi += val2;
        t.assert(verrifyEqual32(bi, val));
    }
}


void minustest(const tTest& t)
{
    {
        i32 val = -9000;
        algo::tBigInteger bi(val);
        while (val < 9000)
        {
            t.assert(verrifyEqual32(bi, val));
            val++;
            bi -= -1;
        }
    }

    {
        i32 val = 9000;
        algo::tBigInteger bi(val);
        while (val > -9000)
        {
            t.assert(verrifyEqual32(bi, val));
            val--;
            bi -= 1;
        }
    }

    for (int i = 0; i < kRandIters; i++)
    {
        i32 val = (rand() % 9000) - 4500;
        algo::tBigInteger bi(val);
        t.assert(verrifyEqual32(bi, val));

        i32 val2 = bigRand();               // <-- could be pos or neg any value
        if ((val > 0 && val2 > 0 && val + val2 <= 0) ||
            (val < 0 && val2 < 0 && val + val2 >= 0))
        {
            // Overflow happened, so we cannot do any testing
            // on this iteration.
            continue;
        }

        val += val2;
        bi -= -val2;
        t.assert(verrifyEqual32(bi, val));
    }
}


void multtest(const tTest& t)
{
    i32 hardcodedA[] = { 1000, 1000, 1000, 1000, 1000, 0,    1,    -1,   2,    -2   };
    i32 hardcodedB[] = { 0,    1,    -1,   2,   -2,    1234, 1234, 1234, 1234, 1234 };

    for (size_t i = 0; i < sizeof(hardcodedA)/sizeof(i32); i++)
    {
        algo::tBigInteger a(hardcodedA[i]);
        algo::tBigInteger b(hardcodedB[i]);
        algo::tBigInteger c = a * b;
        t.assert(verrifyEqual32(a, hardcodedA[i]));
        t.assert(verrifyEqual32(b, hardcodedB[i]));
        t.assert(verrifyEqual32(c, hardcodedA[i] * hardcodedB[i]));
    }

    for (int i = 0; i < kRandIters; i++)
    {
        i32 val1 = bigRand();
        algo::tBigInteger bi1(val1);
        t.assert(verrifyEqual32(bi1, val1));

        i32 val2 = bigRand();
        algo::tBigInteger bi2(val2);
        t.assert(verrifyEqual32(bi2, val2));

        algo::tBigInteger mult = bi1 * bi2;
        i64 correct = ((i64)val1) * ((i64)val2);
        t.assert(verrifyEqual64(mult, correct));
    }
}


void divtest(const tTest& t)
{
    // todo
}


void modtest(const tTest& t)
{
    // todo
}


void equaltest(const tTest& t)
{
    // todo
}


void lesstest(const tTest& t)
{
    // todo
}


int main()
{
    tCrashReporter::init();
    srand(time(0));

    tTest("tBigInteger i32 constructor test", i32constructortest);
    tTest("tBigInteger string constructor test", stringconstructortest);
    tTest("tBigInteger toString test", tostringtest);
    tTest("tBigInteger plus test", plustest);
    tTest("tBigInteger minus test", minustest);
    tTest("tBigInteger mult test", multtest);
    tTest("tBigInteger div test", divtest);
    tTest("tBigInteger mod test", modtest);
    tTest("tBigInteger equal test", equaltest);
    tTest("tBigInteger less test", lesstest);

    return 0;
}
