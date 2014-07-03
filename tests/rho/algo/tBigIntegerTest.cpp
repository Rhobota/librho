#include <rho/algo/tBigInteger.h>
#include <rho/tTest.h>
#include <rho/tCrashReporter.h>
#include <rho/eRho.h>

#include <sstream>
#include <string>

using namespace rho;
using std::cout;
using std::endl;
using std::vector;
using std::string;


static const int kRandIters = 10000;


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


string toString(i32 val)
{
    std::ostringstream out;
    out << val;
    return out.str();
}


int numBits(u64 a)
{
    int i = 0;
    while (a != 0) { a >>= 1; i++; }
    return i;
}


bool willOverflowPlus(i64 a, i64 b)
{
    if (a > 0 && b > 0 && a + b <= 0)
        return true;
    if (a < 0 && b < 0 && a + b >= 0)
        return true;
    return false;
}


bool willOverflowMinus(i64 a, i64 b)
{
    if (a > 0 && b < 0 && a - b <= 0)
        return true;
    if (a < 0 && b > 0 && a - b >= 0)
        return true;
    return false;
}


bool willOverflowMult(i64 a, i64 b)
{
    int abits = numBits(a);
    int bbits = numBits(b);
    return abits + bbits > 63;
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

    for (size_t i = 0; i < v.size(); i++)
    {
        string str = toString(v[i]);
        algo::tBigInteger bi(str);
        t.assert(verrifyEqual32(bi, v[i]));
    }

    for (int i = 0; i < kRandIters; i++)
    {
        i32 val = bigRand();
        string str = toString(val);
        algo::tBigInteger bi(str);
        t.assert(verrifyEqual32(bi, val));
    }
}


void tostringtest(const tTest& t)
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

    for (size_t i = 0; i < v.size(); i++)
    {
        string str = toString(v[i]);
        algo::tBigInteger bi(str);
        t.assert(str == bi.toString());
    }

    for (int i = 0; i < kRandIters/2; i++)
    {
        i32 val = bigRand();
        string str = toString(val);
        algo::tBigInteger bi(str);
        t.assert(str == bi.toString());
    }
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

    for (int i = 0; i < kRandIters; i++)
    {
        i32 val = bigRand();
        algo::tBigInteger bi(val);
        t.assert(verrifyEqual32(bi, val));

        i32 val2 = bigRand();               // <-- could be pos or neg any value

        i64 val3 = ((i64)val) + ((i64)val2);
        bi += val2;
        t.assert(verrifyEqual64(bi, val3));
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

    for (int i = 0; i < kRandIters; i++)
    {
        i32 val = bigRand();
        algo::tBigInteger bi(val);
        t.assert(verrifyEqual32(bi, val));

        i32 val2 = bigRand();               // <-- could be pos or neg any value

        i64 val3 = ((i64)val) - ((i64)val2);
        bi -= val2;
        t.assert(verrifyEqual64(bi, val3));
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
    i32 hardcodedA[] = { 1001, 1001, 1001, 1001, 0,    1,    -1,   2,    -2   };
    i32 hardcodedB[] = { 1,    -1,   2,   -2,    1234, 1234, 1234, 1234, 1234 };

    for (size_t i = 0; i < sizeof(hardcodedA)/sizeof(i32); i++)
    {
        algo::tBigInteger a(hardcodedA[i]);
        algo::tBigInteger b(hardcodedB[i]);
        algo::tBigInteger c = a / b;
        t.assert(verrifyEqual32(a, hardcodedA[i]));
        t.assert(verrifyEqual32(b, hardcodedB[i]));
        t.assert(verrifyEqual32(c, hardcodedA[i] / hardcodedB[i]));
    }

    {
        algo::tBigInteger a(102424);
        algo::tBigInteger b(0);
        try
        {
            algo::tBigInteger c = a / b;
            t.fail();
        }
        catch (eInvalidArgument& e)
        {
            // Yay.
        }
    }

    for (int i = 0; i < kRandIters; i++)
    {
        i32 val1 = bigRand();
        algo::tBigInteger bi1(val1);
        t.assert(verrifyEqual32(bi1, val1));

        i32 val2 = bigRand();
        algo::tBigInteger bi2(val2);
        t.assert(verrifyEqual32(bi2, val2));

        if (val2 == 0)
            continue;

        algo::tBigInteger mult = bi1 / bi2;
        i32 correct = val1 / val2;
        t.assert(verrifyEqual64(mult, correct));
    }
}


void modtest(const tTest& t)
{
    i32 hardcodedA[] = { 1001, 1001, 1001, 1001, -1001, -1001, 0,    1,    -1,   2,    -2   };
    i32 hardcodedB[] = { 1,    -1,   2,   -2,    2,     -2,    1234, 1234, 1234, 1234, 1234 };

    for (size_t i = 0; i < sizeof(hardcodedA)/sizeof(i32); i++)
    {
        algo::tBigInteger a(hardcodedA[i]);
        algo::tBigInteger b(hardcodedB[i]);
        algo::tBigInteger c = a % b;
        t.assert(verrifyEqual32(a, hardcodedA[i]));
        t.assert(verrifyEqual32(b, hardcodedB[i]));
        t.assert(verrifyEqual32(c, hardcodedA[i] % hardcodedB[i]));
    }

    {
        algo::tBigInteger a(102424);
        algo::tBigInteger b(0);
        try
        {
            algo::tBigInteger c = a % b;
            t.fail();
        }
        catch (eInvalidArgument& e)
        {
            // Yay.
        }
    }

    for (int i = 0; i < kRandIters; i++)
    {
        i32 val1 = bigRand();
        algo::tBigInteger bi1(val1);
        t.assert(verrifyEqual32(bi1, val1));

        i32 val2 = bigRand();
        algo::tBigInteger bi2(val2);
        t.assert(verrifyEqual32(bi2, val2));

        if (val2 == 0)
            continue;

        algo::tBigInteger mult = bi1 % bi2;
        i32 correct = val1 % val2;
        t.assert(verrifyEqual64(mult, correct));
    }
}


void bigtest(const tTest& t)
{
    algo::tBigInteger bi(0);
    i64 correct = 0;

    for (int i = 0; i < kRandIters; i++)
    {
        i32 val = bigRand();

        switch (rand() % 5)
        {
            case 0:  // plus
                if (!willOverflowPlus(correct, val))
                {
                    bi += val;
                    correct += val;
                }
                break;

            case 1:  // minus
                if (!willOverflowMinus(correct, val))
                {
                    bi -= val;
                    correct -= val;
                }
                break;

            case 2:  // mult
                if (!willOverflowMult(correct, val))
                {
                    bi *= val;
                    correct *= val;
                }
                break;

            case 3:  // divide
                if (val != 0)
                {
                    bi /= val;
                    correct /= val;
                }
                break;

            case 4:  // mod
                if (val != 0)
                {
                    bi %= val;
                    correct %= val;
                }
                break;

            default:
                throw "bad!";
        }

        t.assert(verrifyEqual64(bi, correct));
    }
}


void equaltest(const tTest& t)
{
    for (i32 i = -4000; i <= 4000; i++)
    {
        algo::tBigInteger a(i);
        for (i32 j = -1000; j <= 1000; j++)
        {
            algo::tBigInteger b(j);
            bool biEqual = a == b;
            bool i32Equal = i == j;
            t.assert(biEqual == i32Equal);
        }
    }
}


void lesstest(const tTest& t)
{
    for (i32 i = -4000; i <= 4000; i++)
    {
        algo::tBigInteger a(i);
        for (i32 j = -1000; j <= 1000; j++)
        {
            algo::tBigInteger b(j);
            bool biLess = a < b;
            bool i32Less = i < j;
            t.assert(biLess == i32Less);
        }
    }
}


void pseudoPrimeTest(const tTest& t)
{
    // empty... hand tested...
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
    tTest("tBigInteger final big test", bigtest);
    tTest("tBigInteger equal test", equaltest);
    tTest("tBigInteger less test", lesstest);
    tTest("tBigInteger pseudo prime test", pseudoPrimeTest);

    return 0;
}
