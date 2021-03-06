#include <cstdlib>


namespace rho
{
namespace algo
{


template <class A, class B>
std::vector< std::pair<A,B> > zip(const std::vector<A>& a, const std::vector<B>& b)
{
    if (a.size() != b.size())
        throw eInvalidArgument("The vectors must be the same size to zip them together.");
    std::vector< std::pair<A,B> > z(a.size());
    for (std::size_t i = 0; i < a.size(); i++)
        z[i] = std::make_pair(a[i], b[i]);
    return z;
}


template <class A, class B>
std::vector< std::pair<A,B> > zip(const std::vector<A>& a, const B& b)
{
    std::vector< std::pair<A,B> > z(a.size());
    for (std::size_t i = 0; i < a.size(); i++)
        z[i] = std::make_pair(a[i], b);
    return z;
}


template <class A, class B>
void unzip(const std::vector< std::pair<A,B> >& zipped, std::vector<A>& firstPart,
                                                        std::vector<B>& secondPart)
{
    firstPart.clear();
    secondPart.clear();
    for (size_t i = 0; i < zipped.size(); i++)
    {
        firstPart.push_back(zipped[i].first);
        secondPart.push_back(zipped[i].second);
    }
}


template <class A>
void shuffle(std::vector<A>& v, iLCG& lcg)
{
    if (v.size() == 0)
        return;
    for (std::size_t i = v.size()-1; i > 0; i--)
    {
        u64 rr = lcg.next();
        u32 r = (u32)(rr % (i+1));
        std::swap(v[i], v[r]);
    }
}


template <class A, class B>
void shuffle(std::vector<A>& a, std::vector<B>& b, iLCG& lcg)
{
    if (a.size() != b.size())
        throw eInvalidArgument("The vectors must be the same size to shuffle them in the same way.");
    if (a.size() == 0)
        return;
    for (std::size_t i = a.size()-1; i > 0; i--)
    {
        u64 rr = lcg.next();
        u32 r = (u32)(rr % (i+1));
        std::swap(a[i], a[r]);
        std::swap(b[i], b[r]);
    }
}


template <class A>
std::vector<A> mix(const std::vector<A>& a, const std::vector<A>& b, iLCG& lcg)
{
    std::vector<A> m;
    for (std::size_t i = 0; i < a.size(); i++)
        m.push_back(a[i]);
    for (std::size_t i = 0; i < b.size(); i++)
        m.push_back(b[i]);
    shuffle(m, lcg);
    return m;
}


}  // namespace algo
}  // namespace rho
