#ifndef __rho_sync_iSync_h__
#define __rho_sync_iSync_h__


namespace rho
{
namespace sync
{


class iSync
{
    public:

        /**
         * Blocks until the synchronization object can be acquired,
         * then acquires the sync-object and returns.
         */
        virtual void acquire() = 0;

        /**
         * Releases the synchronization object.
         */
        virtual void release() = 0;

        virtual ~iSync() { }
};


}    // namespace sync
}    // namespace rho


#endif    // __rho_sync_iSync_h__
