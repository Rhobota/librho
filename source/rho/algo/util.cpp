#include <rho/algo/string_util.h>
#include <rho/algo/stat_util.h>

#include <sstream>
using namespace std;


namespace rho
{
namespace algo
{


string findDirName(string path)
{
    if (path.length() == 0)
        return "";
    int i = (int) path.length()-1;
    for (; i >= 0; i--)
        if (path[i] == '/' || path[i] == '\\')
            break;
    return path.substr(0, i+1);
}

string stripComment(string str, string leader)
{
    size_t p = str.find(leader);
    if (p == string::npos)
        return str;
    return str.substr(0, p);
}

string trim(string str)
{
    // Note: isspace() returns true on all whitespace

    size_t start = 0;
    while (start < str.length() && isspace(str[start])) start++;
    if (start == str.length()) return "";

    size_t end = str.length()-1;
    while (isspace(str[end])) end--;

    return str.substr(start, end+1-start);
}

bool containsWhitespace(string str)
{
    for (size_t i = 0; i < str.length(); i++)
        if (isspace(str[i]))
            return true;
    return false;
}

string replace(string str, string from, string to)
{
    size_t pos = 0;
    size_t len = from.length();
    while ((pos = str.find(from, pos)) != string::npos)
    {
        str.replace(pos, len, to);
        pos += to.length();
    }
    return str;
}

vector<string> split(string str, string delim, int maxparts)
{
    // Handle when str has no contents.
    if (str.length() == 0)
        return vector<string>(1, "");

    // Handle when delim has no contents.
    int extra = (delim.size() > 0) ? 0 : 1;

    // Handle the maxparts countdown.
    size_t countdown;
    if (maxparts < 1)
        countdown = str.length();
    else
        countdown = maxparts;     // countdown is always >= 1

    // Keep track of where in str we are.
    size_t curr = 0;

    // Build this as we go:
    vector<string> parts;

    // While two or more parts remaining and some of str left to consume...
    bool endedinsplit = true;
    for (; countdown > 1 && curr < str.length(); --countdown)
    {
        size_t loc = str.find(delim, curr);
        if (loc != string::npos)
        {
            string sub = str.substr(curr, loc+extra-curr);
            curr = loc+extra+delim.size();
            parts.push_back(sub);
        }
        else
        {
            string sub = str.substr(curr);
            curr = str.length();
            parts.push_back(sub);
            endedinsplit = false;
            break;
        }
    }

    // Note: (countdown > 0) is guaranteed at this point.

    // If anything remains in str, consume it. (happens when countdown gets to one in loop above)
    if (curr < str.length())
        parts.push_back(str.substr(curr));

    // Else ended because curr==str.length(). If the last delim we found was right at the end of str...
    else if (endedinsplit)
        parts.push_back("");

    return parts;
}

vector<string> removeEmptyParts(vector<string> parts)
{
    vector<string> parts2;
    for (size_t i = 0; i < parts.size(); i++)
        if (parts[i].length() > 0)
            parts2.push_back(parts[i]);
    return parts2;
}

double toDouble(string str, bool* errorFlag)
{
    istringstream in(str);
    double d = 0.0;
    if (!(in >> d))
        *errorFlag = true;
    return d;
}

int toInt(string str, bool* errorFlag)
{
    istringstream in(str);
    int i = 0;
    if (!(in >> i))
        *errorFlag = true;
    return i;
}

f64 nrand(iLCG& lcg)
{
    // Uses the Central Limit Theorem to generate a random number
    // from the Normal Distribution (mean-zero, stddev-one).
    static const int kNumToSum = 40;

    static const f64 kNumToSumDbl = (f64)kNumToSum;
    static const f64 kSqrt12 = std::sqrt(12.0);
    static const f64 kSqrtNum = std::sqrt(kNumToSumDbl);
    static const f64 kMultVal = kSqrt12 * kSqrtNum / kNumToSumDbl;

    f64 sum = 0.0;
    for (int i = 0; i < kNumToSum; i++)
    {
        f64 r = ((f64)lcg.next()) / ((f64)lcg.randMax());
        r -= 0.5;
        sum += r;
    }
    return sum * kMultVal;
}


}   // namespace algo
}   // namespace rho
