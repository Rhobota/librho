#ifndef __rho_sync_tCondition_h__
#define __rho_sync_tCondition_h__


#include <rho/ppcheck.h>
#include <rho/bNonCopyable.h>
#include <rho/sync/tMutex.h>
#include <rho/sysinclude.h>


namespace rho
{
namespace sync
{


class tCondition : public bNonCopyable
{
    public:

        tCondition(tMutex& mux);

        ~tCondition();

        void wait(tMutex& mux);

        void broadcastAll(tMutex& mux);

    private:

        tMutex& m_mux;
        pthread_cond_t m_cond;
};


}   // namespace sync
}   // namespace rho


#endif
