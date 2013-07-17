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
        z[i] = make_pair(a[i], b[i]);
    return z;
}


template <class A, class B>
std::vector< std::pair<A,B> > zip(const std::vector<A>& a, const B& b)
{
    std::vector< std::pair<A,B> > z(a.size());
    for (std::size_t i = 0; i < a.size(); i++)
        z[i] = make_pair(a[i], b);
    return z;
}


template <class A>
void shuffle(std::vector<A>& v)
{
    if (v.size() == 0)
        return;
    for (std::size_t i = v.size()-1; i > 0; i--)
    {
        int r = rand() % (i+1);
        swap(v[i], v[r]);
    }
}


template <class A>
std::vector<A> mix(const std::vector<A>& a, const std::vector<A>& b)
{
    std::vector<A> m;
    for (std::size_t i = 0; i < a.size(); i++)
        m.push_back(a[i]);
    for (std::size_t i = 0; i < b.size(); i++)
        m.push_back(b[i]);
    shuffle(m);
    return m;
}


}  // namespace algo
}  // namespace rho
