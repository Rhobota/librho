#ifndef __rho_sync_iSync_h__
#define __rho_sync_iSync_h__


namespace rho
{
namespace sync
{


class iSync
{
    public:

        virtual void acquire() = 0;
        virtual void release() = 0;

        virtual ~iSync() { }
};


}    // namespace sync
}    // namespace rho


#endif    // __rho_sync_iSync_h__
