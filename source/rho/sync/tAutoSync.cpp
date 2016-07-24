#include <rho/sync/tAutoSync.h>


namespace rho
{
namespace sync
{


tAutoSync::tAutoSync(const iSync& s)
    : m_s(s)
{
    s.acquire();
}

tAutoSync::~tAutoSync()
{
    m_s.release();
}


}   // namespace sync
}   // namespace rho
