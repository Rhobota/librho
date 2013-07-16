#include <rho/img/tCurrentFrame.h>


namespace rho
{
namespace img
{


class tFrameGettingThread : public rho::sync::iRunnable
{
    public:

        tFrameGettingThread(tCurrentFrame* cf)
            : m_cf(cf)
        {
        }

        void run()
        {
            rho::img::tImage image;
            while (! m_cf->m_stop)
            {
                m_cf->m_imageCap->getFrame(&image);

                if (m_cf->m_obs)
                    m_cf->m_obs->gotFrame(&image);

                {
                    rho::sync::tAutoSync as(m_cf->m_mutex);
                    image.copyTo(&(m_cf->m_frame));
                    m_cf->m_gotFirst = true;
                }
            }
        }

    private:

        tCurrentFrame* m_cf;
};


tCurrentFrame::tCurrentFrame(rho::refc<rho::img::iImageCap> imageCap,
        iCurrentFameObserver* obs)
    : m_imageCap(imageCap), m_obs(obs), m_gotFirst(false), m_stop(false)
{
    m_thread = new rho::sync::tThread(rho::refc<rho::sync::iRunnable>(new tFrameGettingThread(this)));
}

void tCurrentFrame::getCurrentFrame(rho::img::tImage* image)
{
    while (! m_gotFirst)
        rho::sync::tThread::msleep(10);

    rho::sync::tAutoSync as(m_mutex);
    m_frame.copyTo(image);
}

rho::img::tImageCapParams tCurrentFrame::getParams()
{
    return m_imageCap->getParams();
}

void tCurrentFrame::stop()
{
    m_stop = true;
    if (m_thread)
    {
        m_thread->join();
        m_thread = NULL;
    }
}

tCurrentFrame::~tCurrentFrame()
{
    stop();
}


}   // namespace img
}   // namespace rho
