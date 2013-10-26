#include <rho/algo/vector_util.h>
#include <rho/algo/string_util.h>
#include <rho/tTest.h>
#include <rho/tCrashReporter.h>

#include <vector>

using namespace rho;
using std::cout;
using std::endl;
using std::vector;
using std::pair;
using std::string;


void vectorUtilTest(const tTest& t)
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


bool test(string str, string v[], int size)
{
    std::vector<string> correct;
    for (int i = 0; i < size; i++)
        correct.push_back(v[i]);
    return algo::split(str, "/") == correct;
}


void stringUtilTest(const tTest& t)
{
    { string v[] = {""};
    t.assert(test("", v, 1)); }

    { string v[] = {     "",""};
    t.assert(test("/", v, 2)); }
    { string v[] = {     "a"};
    t.assert(test("a", v, 1)); }

    { string v[] = {    "","a"};
    t.assert(test("/a", v, 2)); }
    { string v[] = {    "a",""};
    t.assert(test("a/", v, 2)); }
    { string v[] = {    "","",""};
    t.assert(test("//", v, 3)); }
    { string v[] = {    "aa"};
    t.assert(test("aa", v, 1)); }

    { string v[] = {   "","","a"};
    t.assert(test("//a", v, 3)); }
    { string v[] = {   "","a",""};
    t.assert(test("/a/", v, 3)); }
    { string v[] = {   "","","",""};
    t.assert(test("///", v, 4)); }
    { string v[] = {   "","aa"};
    t.assert(test("/aa", v, 2)); }
    { string v[] = {   "a","a"};
    t.assert(test("a/a", v, 2)); }
    { string v[] = {   "aa",""};
    t.assert(test("aa/", v, 2)); }
    { string v[] = {   "a","",""};
    t.assert(test("a//", v, 3)); }
    { string v[] = {   "aaa"};
    t.assert(test("aaa", v, 1)); }

    { string v[] = {   "","","a",""};
    t.assert(test("//a/", v, 4)); }
    { string v[] = {   "","a","",""};
    t.assert(test("/a//", v, 4)); }
    { string v[] = {   "","","","",""};
    t.assert(test("////", v, 5)); }
    { string v[] = {   "","aa",""};
    t.assert(test("/aa/", v, 3)); }
    { string v[] = {   "a","a",""};
    t.assert(test("a/a/", v, 3)); }
    { string v[] = {   "aa","",""};
    t.assert(test("aa//", v, 3)); }
    { string v[] = {   "a","","",""};
    t.assert(test("a///", v, 4)); }
    { string v[] = {   "aaa",""};
    t.assert(test("aaa/", v, 2)); }
    { string v[] = {   "","","aa"};
    t.assert(test("//aa", v, 3)); }
    { string v[] = {   "","a","a"};
    t.assert(test("/a/a", v, 3)); }
    { string v[] = {   "","","","a"};
    t.assert(test("///a", v, 4)); }
    { string v[] = {   "","aaa"};
    t.assert(test("/aaa", v, 2)); }
    { string v[] = {   "a","aa"};
    t.assert(test("a/aa", v, 2)); }
    { string v[] = {   "aa","a"};
    t.assert(test("aa/a", v, 2)); }
    { string v[] = {   "a","","a"};
    t.assert(test("a//a", v, 3)); }
    { string v[] = {   "aaaa"};
    t.assert(test("aaaa", v, 1)); }
}


int main()
{
    tCrashReporter::init();

    tTest("vector util test", vectorUtilTest);
    tTest("string util test", stringUtilTest);

    return 0;
}
