#include <rho/algo/vector_util.h>
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
    vector<int> a;
    for (int i = 0; i < 30; i++)
        a.push_back(i);

    vector<double> b;
    for (int i = 0; i < 30; i++)
        b.push_back(i+1);

    vector< pair<int,double> > zipped1 = algo::zip(a, b);
    vector< pair<int,double> > zipped2 = algo::zip(a, 4.0);

    vector<int> a2;
    vector<double> b2;
    algo::unzip(zipped1, a2, b2);

    t.assert(a == a2);
    t.assert(b == b2);

    algo::shuffle(zipped2);

    vector< pair<int,double> > all = algo::mix(zipped1, zipped2);
}


int main()
{
    tCrashReporter::init();

    tTest("Everything test", allTest);

    return 0;
}
