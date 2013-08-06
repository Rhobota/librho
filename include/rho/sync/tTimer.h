#ifndef __rho_sync_tTimer_h__
#define __rho_sync_tTimer_h__


#include <rho/sync/iTimerObserver.h>
#include <rho/sync/tThread.h>
#include <rho/eRho.h>
#include <rho/bNonCopyable.h>
#include <rho/types.h>


namespace rho
{
namespace sync
{


class tTimer : public bNonCopyable
{
    public:

        /**
         * Create a thread that will call the given observer periodically
         * and repeatedly.
         * This timer does not own the observer (i.e. you must destroy it
         * yourself).
         *
         * 'period' is in micro-seconds (10^-6 seconds).
         */
        tTimer(iTimerObserver* obs, u64 period);

        /**
         * Returns the timer's observer.
         */
        iTimerObserver* getObserver() const;

        /**
         * Returns the timer's period in micro-seconds (10^-6 seconds).
         */
        u64 getPeriod() const;

        /**
         * Stops the timer (i.e. thread) from calling the observer ever again!
         * It is safe to destroy the observer after this method returns.
         */
        void stop();

        /**
         * Returns true if the timer has been cancelled. This timer is useless
         * if it has been cancelled, because it cannot be started again.
         * Create a new timer if you need to begin it again.
         */
        bool isStopped() const;

        /**
         * Destructor... If the timer is not canceled, it will be cancelled here.
         */
        ~tTimer();

        /**
         * Returns the current system time in micro-seconds (10^-6)
         */
        static u64 usecTime();

    private:

        refc<tThread> m_thread;

        iTimerObserver* m_obs;
        u64 m_period;
        u64 m_nextCallTime;

        bool m_running;
        bool m_stopped;

        friend class tTimerThread;
};


}  // namespace sync
}  // namespace rho


#endif  // __rho_sync_tTimer_h__
