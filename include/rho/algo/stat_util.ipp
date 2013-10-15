
template <class T>
T mean(const std::vector<T>& v)
{
    if (v.size() == 0)
        throw eInvalidArgument("The mean is not defined over zero samples.");
    T sum(0);
    for (size_t i = 0; i < v.size(); i++)
        sum += v[i];
    sum /= T(v.size());
    return sum;
}

template <class T>
T mean(const std::vector< std::vector<T> >& m)
{
    T sum(0);
    u32 count = 0;
    for (size_t i = 0; i < m.size(); i++)
    {
        for (size_t j = 0; j < m[i].size(); j++)
            sum += m[i][j];
        count += m[i].size();
    }
    if (count == 0)
        throw eInvalidArgument("The mean is not defined over zero samples.");
    sum /= T(count);
    return sum;
}

template <class T>
T variance(const std::vector<T>& v)
{
    if (v.size() == 0)
        throw eInvalidArgument("The variance is not defined over zero samples.");
    if (v.size() == 1)
        throw eInvalidArgument("The variance is not defined over one sample.");
    T av = mean(v);
    T sum(0);
    for (size_t i = 0; i < v.size(); i++)
        sum += (v[i]-av)*(v[i]-av);
    sum /= T(v.size()-1);
    return sum;
}

template <class T>
T variance(const std::vector< std::vector<T> >& m)
{
    T sum(0);
    u32 count = 0;
    T av = mean(m);
    for (size_t i = 0; i < m.size(); i++)
    {
        for (size_t j = 0; j < m[i].size(); j++)
            sum += (m[i][j]-av)*(m[i][j]-av);
        count += m[i].size();
    }
    if (count == 0)
        throw eInvalidArgument("The variance is not defined over zero samples.");
    if (count == 1)
        throw eInvalidArgument("The variance is not defined over one sample.");
    sum /= T(count-1);
    return sum;
}

template <class T>
T stddev(const std::vector<T>& v)
{
    return std::sqrt(variance(v));
}

template <class T>
T stddev(const std::vector< std::vector<T> >& m)
{
    return std::sqrt(variance(m));
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
