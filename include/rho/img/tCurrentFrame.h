#ifndef __rho_img_tCurrentFrame_h__
#define __rho_img_tCurrentFrame_h__


#include <rho/bNonCopyable.h>
#include <rho/img/tImage.h>
#include <rho/img/tImageCapFactory.h>
#include <rho/sync/tAutoSync.h>
#include <rho/sync/tMutex.h>
#include <rho/sync/tThread.h>


namespace rho
{
namespace img
{


class iCurrentFameObserver
{
    public:

        virtual void gotFrame(rho::img::tImage* image) = 0;

        virtual ~iCurrentFameObserver() {}
};


class tCurrentFrame : public rho::bNonCopyable
{
    public:

        tCurrentFrame(rho::refc<rho::img::iImageCap> imageCap,
                iCurrentFameObserver* obs = NULL);

        void getCurrentFrame(rho::img::tImage* image);

        rho::img::tImageCapParams getParams();

        void stop();   // returns after the internal thread has halted

        ~tCurrentFrame();

    private:

        rho::refc<rho::img::iImageCap> m_imageCap;
        iCurrentFameObserver*          m_obs;
        rho::img::tImage               m_frame;
        rho::sync::tMutex              m_mutex;
        rho::refc<rho::sync::tThread>  m_thread;
        bool                           m_gotFirst;
        bool                           m_stop;

        friend class tFrameGettingThread;
};


}   // namespace img
}   // namespace rho


#endif // __rho_img_tCurrentFrame_h__
