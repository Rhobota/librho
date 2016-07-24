#include <rho/sync/tCondition.h>
#include <rho/sync/ebSync.h>


namespace rho
{
namespace sync
{


tCondition::tCondition(tMutex& mux)
    : m_mux(mux),
      m_cond()
{
    if (pthread_cond_init(&m_cond, NULL) != 0)
    {
        throw eConditionCreationError();
    }
}

tCondition::~tCondition()
{
    pthread_cond_destroy(&m_cond);
}

void tCondition::wait(tMutex& mux)
{
    if (&mux != &m_mux)
        throw eRuntimeError("Mux mismatch");
    if (pthread_cond_wait(&m_cond, &mux.m_mutex) != 0)
        throw eRuntimeError("Why can't I wait on the condition?");
}

void tCondition::broadcastAll(tMutex& mux)
{
    if (&mux != &m_mux)
        throw eRuntimeError("Mux mismatch");
    if (pthread_cond_broadcast(&m_cond) != 0)
        throw eRuntimeError("Why can't I broadcast?");
}


}  // namespace sync
}  // namespace rho
