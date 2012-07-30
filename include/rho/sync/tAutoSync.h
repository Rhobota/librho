#ifndef __rho_sync_tAutoSync_h__
#define __rho_sync_tAutoSync_h__


#include <rho/bNonCopyable.h>

#include <rho/sync/iSync.h>


namespace rho
{
namespace sync
{


class tAutoSync : public bNonCopyable
{
    public:

        /**
         * Calls s.acquire()
         */
        tAutoSync(iSync& s);

        /**
         * Calls s.release()
         */
        ~tAutoSync();

    private:

        iSync& m_s;
};


}   // namespace sync
}   // namespace rho


#endif    // __rho_sync_tAutoSync_h__
