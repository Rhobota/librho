#ifndef __rho_sync_iRunnable_h__
#define __rho_sync_iRunnable_h__


namespace rho
{
namespace sync
{


class iRunnable
{
    public:

        /**
         * See tThread
         */
        virtual void run() = 0;

        virtual ~iRunnable() { }
};


}     // namespace sync
}     // namespace rho


#endif    // __rho_sync_iRunnable_h__
