#include <rho/algo/tLCG.h>
#include <rho/tTest.h>
#include <rho/tCrashReporter.h>

using namespace rho;
using std::cout;
using std::endl;


void defaultLCG(const tTest& t)
{
    algo::tLCG lcg;
    t.assert(lcg.next() == 1103527590);
    t.assert(lcg.next() == 377401575);
    t.assert(lcg.next() == 662824084);
    t.assert(lcg.next() == 1147902781);
    t.assert(lcg.next() == 2035015474);
    t.assert(lcg.next() == 368800899);
    t.assert(lcg.next() == 1508029952);
    t.assert(lcg.randMax() == 2147483647);
}


void defaultFairTest(const tTest& t)
{
    algo::tLCG lcg(1);

    const u32 kNumBins = 100;
    u32 bins[kNumBins];
    for (u32 i = 0; i < kNumBins; i++)
        bins[i] = 0;

    for (u32 i = 0; i < 1234567890; i++)
        bins[(lcg.next()%kNumBins)]++;

    for (u32 i = 0; i < kNumBins; i++)
    {
        for (u32 j = 0; j < kNumBins; j++)
            t.assert((bins[i]>bins[j]?bins[i]-bins[j]:bins[j]-bins[i]) <= 20000);
    }
}


void knuthFairTest(const tTest& t)
{
    algo::tKnuthLCG lcg(1);

    const u32 kNumBins = 100;
    u32 bins[kNumBins];
    for (u32 i = 0; i < kNumBins; i++)
        bins[i] = 0;

    for (u32 i = 0; i < 1234567890; i++)
        bins[(lcg.next()%kNumBins)]++;

    for (u32 i = 0; i < kNumBins; i++)
    {
        for (u32 j = 0; j < kNumBins; j++)
            t.assert((bins[i]>bins[j]?bins[i]-bins[j]:bins[j]-bins[i]) <= 20000);
    }
}


int main()
{
    tCrashReporter::init();

    tTest("Default LCG test", defaultLCG, 1000);
    tTest("Default LCG fair test", defaultFairTest);
    tTest("Knuth LCG fair test", knuthFairTest);

    return 0;
}
