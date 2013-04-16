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


void test(const tTest& t)
{
    {
        i32 val = 0x7FFFFFFF;
        cout << "val  = " << val << endl;
        cout << "-val = " << -val << endl;
        algo::tBigInteger bi(val);
        print(bi);
    }

    {
        i32 val = 0x80000000;
        cout << "val  = " << val << endl;
        cout << "-val = " << -val << endl;
        algo::tBigInteger bi(val);
        print(bi);
    }

    {
        algo::tBigInteger bi("2147483647");
        print(bi);
    }
}


int main()
{
    tCrashReporter::init();

    tTest("tBigInteger test", test);

    return 0;
}
