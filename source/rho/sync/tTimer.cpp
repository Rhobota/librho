#include <rho/sync/tTimer.h>

#include <sys/time.h>


namespace rho
{
namespace sync
{


const static u64 kMinSleepTime = 300;    // 0.3 milliseconds
const static u64 kMaxSleeptime = 50000;  // 50.0 milliseconds


class tTimerInitEndManager
{
    public:

        tTimerInitEndManager(iTimerObserver* obs)
            : m_obs(obs)
        {
            m_obs->timerThreadInit();
        }

        ~tTimerInitEndManager()
        {
            m_obs->timerThreadEnd();
        }

    private:

        iTimerObserver* m_obs;
};


class tTimerThread : public iRunnable, public bNonCopyable
{
    public:

        tTimerThread(tTimer* timer)
            : m_timer(timer)
        {
        }

        void run()
        {
            tTimerInitEndManager initEndManager(m_timer->m_obs);

            while (m_timer->m_running)
            {
                u64 currtime = tTimer::usecTime();

                // If it's time to call the observer...
                if (currtime >= m_timer->m_nextCallTime)
                {
                    bool callAgain = m_timer->m_obs->timerAlert(m_timer->m_period);
                    if (!callAgain)
                        break;
                    m_timer->m_nextCallTime += m_timer->m_period;
                }

                // Else, we need to wait a little longer...
                else
                {
                    u64 waitTime = m_timer->m_nextCallTime - currtime;
                    if (waitTime > kMinSleepTime)
                    {
                        waitTime = std::min(waitTime-kMinSleepTime, kMaxSleeptime);
                        tThread::usleep(waitTime);
                    }
                }
            }

            m_timer->m_stopped = true;
        }

    private:

        tTimer* m_timer;
};


tTimer::tTimer(iTimerObserver* obs, u64 period)
    : m_obs(obs), m_period(period), m_running(true), m_stopped(false)
{
    if (obs == NULL)
        throw eNullPointer("The observer must not be NULL.");
    if (period == 0)
        throw eInvalidArgument("The period must be positive (i.e. not zero).");

    m_nextCallTime = m_period + tTimer::usecTime();

    m_thread = new tThread(refc<iRunnable>(new tTimerThread(this)));
}

iTimerObserver* tTimer::getObserver() const
{
    return m_obs;
}

u64 tTimer::getPeriod() const
{
    return m_period;
}

void tTimer::stop()
{
    if (m_running)
    {
        m_running = false;
        m_thread->join();
        m_thread = NULL;
    }
}

bool tTimer::isStopped() const
{
    return m_stopped;
}

tTimer::~tTimer()
{
    stop();
}

u64 tTimer::usecTime()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);

    u64 currtime = (u64)tv.tv_sec;
    currtime *= 1000000;
    currtime += (u64)tv.tv_usec;

    return currtime;
}


}  // namespace sync
}  // namespace rho
