#include <rho/algo/stat_util.h>
#include <rho/tTest.h>
#include <rho/tCrashReporter.h>

#include <vector>

using namespace rho;
using std::cout;
using std::endl;
using std::vector;
using std::pair;


void allTest(const tTest& t)
{
    {
        vector<int> a;
        for (int i = 0; i < 30; i++)
            a.push_back(i);

        vector<double> b;
        for (int i = 0; i < 30; i++)
            b.push_back(i+1);

        int meani = algo::mean(a);
        int vari  = algo::variance(a);
        int stddi = algo::stddev(a);

        double meand = algo::mean(b);
        double vard  = algo::variance(b);
        double stddd = algo::stddev(b);

        t.assert(meani == 14);
        t.assert(vari == 77);
        t.assert(stddi == 8);

        t.assert(meand == 15.5);
        t.assert(vard == 77.5);
        t.assert(stddd > 8.80 && stddd < 8.81);
    }

    {
        vector< vector<int> > a;
        for (int i = 0; i < 30; i++)
        {
            a.push_back(vector<int>());
            for (int j = 0; j < 10; j++)
                a[i].push_back(j*7+i);
        }

        vector< vector<double> > b;
        for (int i = 0; i < 30; i++)
        {
            b.push_back(vector<double>());
            for (int j = 0; j < 10; j++)
                b[i].push_back(j*7+i);
        }

        int meani = algo::mean(a);
        int vari  = algo::variance(a);
        int stddi = algo::stddev(a);

        double meand = algo::mean(b);
        double vard  = algo::variance(b);
        double stddd = algo::stddev(b);

        t.assert(meani == 46);
        t.assert(vari == 480);
        t.assert(stddi == 21);

        t.assert(meand == 46.0);
        t.assert(vard > 480.769 && vard < 480.77);
        t.assert(stddd > 21.92 && stddd < 21.93);
    }

    {
        vector<double> nvals(100000, 0.0);
        for (int i = 0; i < 100000; i++)
            nvals[i] = algo::nrand();      // <-- uses algo::tKnuthLCG by default

        double a = algo::mean(nvals);
        double stdd = algo::stddev(nvals);

        t.assert(a > -0.005 && a < 0.005);
        t.assert(stdd > 0.999 && stdd < 1.001);
    }
}


int main()
{
    tCrashReporter::init();

    tTest("Everything test", allTest);

    return 0;
}
