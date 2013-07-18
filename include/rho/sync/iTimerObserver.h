#ifndef __rho_sync_iTimerObserver_h__
#define __rho_sync_iTimerObserver_h__


#include <rho/types.h>


namespace rho
{
namespace sync
{


class iTimerObserver
{
    public:

        /**
         * Called when the registered timer fires.
         *
         * The receiver should return true if the timer
         * should continue calling periodically.
         *
         * The receiver should return false if this
         * is the last time it should be called by
         * the timer.
         */
        virtual bool timerAlert(u64 timerPeriod) = 0;

        virtual ~iTimerObserver() { }
};


}   // namespace sync
}   // namespace rho


#endif   // __rho_sync_iTimerObserver_h__
