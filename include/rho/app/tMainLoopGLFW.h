#ifndef __rho_app_tMainLoopGLFW_h__
#define __rho_app_tMainLoopGLFW_h__


#include <rho/app/tWindowGLFW.h>
#include <rho/sync/tPCQ.h>

#include <set>


namespace rho
{
namespace app
{


class tMainLoopGLFW
{
    public:

        /**
         * Constructs the GLFW main loop object.
         * You may only construct one of these
         * at a time, and you may only construct
         * these from the main thread.
         */
        tMainLoopGLFW();

        /**
         * Waits for all opened windows to close,
         * then destructs the main loop.
         * This must be called from the main thread.
         */
        ~tMainLoopGLFW();

        /**
         * Adds a window to the main loop. The window
         * is opened on the next call to once().
         * This method may be called from any thread.
         */
        void addWindow(refc<tWindowGLFW> window);

        /**
         * Returns the number of windows currently registered
         * with the main loop.
         * May be called from any thread.
         */
        u32  numWindows();

        /**
         * Opens new windows, closes old windows, and polls for
         * input events.
         * May only be called on the main thread.
         */
        void once();

        /**
         * Calls once() while there are windows registered with
         * the loop. When all windows are closed, this method
         * returns.
         * May only be called on the main thread.
         */
        void loop();

    private:

        std::set< refc<tWindowGLFW> >   m_windows;
        sync::tPCQ< refc<tWindowGLFW> > m_newWindowsQueue;
};


}   // namespace app
}   // namespace rho


#endif  // __rho_app_tMainLoopGLFW_h__
