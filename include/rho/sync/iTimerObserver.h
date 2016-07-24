#ifndef __rho_sync_iTimerObserver_h__
#define __rho_sync_iTimerObserver_h__


#include <rho/ppcheck.h>
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
         * 'timerPeriod' is in micro-seconds (10^-6 seconds).
         *
         * The receiver should return true if the timer
         * should continue calling periodically.
         *
         * The receiver should return false if this
         * is the last time it should be called by
         * the timer.
         */
        virtual bool timerAlert(u64 timerPeriod) = 0;

        /**
         * Called when the internal timer thread starts up.
         * This is useful if you need to do a per-thread
         * initialization of some sort in your application.
         */
        virtual void timerThreadInit() { }

        /**
         * Called when the internal timer thread is about to
         * terminate. This is useful if you need to tear down
         * what you did in timerThreadInit().
         */
        virtual void timerThreadEnd() { }

        virtual ~iTimerObserver() { }
};


}   // namespace sync
}   // namespace rho


#endif   // __rho_sync_iTimerObserver_h__
