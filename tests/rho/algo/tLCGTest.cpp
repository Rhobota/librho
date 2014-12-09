#include <rho/algo/tLCG.h>
#include <rho/tTest.h>
#include <rho/tCrashReporter.h>

using namespace rho;
using std::cout;
using std::endl;


static const i64 kNumTrials = 2*3*4*5*6*7*8*9*10*11;
static const int kMaxBins = 11;


//
// These chi-square thresholds are for p=0.005
// (taken from http://sites.stat.psu.edu/~mga/401/tables/Chi-square-table.pdf).
//
// Indices indicate number of bins (aka, df+1).
// Indices 0 and 1 are meaningless, but we have to put something there because
// this is a program.
//
static const double kChiTable[] = {
    0.0,      // meaningless
    0.0,      // meaningless
    7.879,    // df=1
    10.597,   // df=2
    12.838,   // df=3
    14.860,   // df=4
    16.750,   // df=5
    18.548,   // df=6
    20.278,   // df=7
    21.955,   // df=8
    23.589,   // df=9
    25.188,   // df=10
    26.757,   // df=11
    28.300,   // df=12
    29.819,   // df=13
    31.319,   // df=14
    32.801,   // df=15
};


void binTest(const tTest& t, algo::iLCG& lcg, size_t numBins, int seed)
{
    std::vector<i64> bins(numBins, 0);
    for (i64 i = 0; i < kNumTrials; i++)
    {
        bins[ lcg.next() % numBins ]++;
    }

    double chiSquare = 0;
    for (size_t i = 0; i < bins.size(); i++)
    {
        double expected = ((double)kNumTrials) / numBins;
        double diff = expected - bins[i];
        chiSquare += diff*diff / expected;
    }

    if (chiSquare > kChiTable[numBins])
    {
        cout << "Fail! seed = " << seed << endl;

        cout << "values:";
        for (size_t i = 0; i < bins.size(); i++)
            cout << " " << bins[i];
        cout << endl;

        cout << "chi-square:  " << chiSquare << endl;
        cout << "table value: " << kChiTable[numBins] << endl;

        t.fail();
    }
}


void binTest(const tTest& t)
{
    for (int i = 2; i <= kMaxBins; i++)
    {
        int seed = rand();
        algo::tLCG lcg(seed);
        binTest(t, lcg, i, seed);
    }

    for (int i = 2; i <= kMaxBins; i++)
    {
        int seed = rand();
        algo::tKnuthLCG lcg(seed);
        binTest(t, lcg, i, seed);
    }
}


int main()
{
    tCrashReporter::init();

    srand(time(0));

    tTest("Bin test", binTest);

    return 0;
}
