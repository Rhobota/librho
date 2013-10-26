#ifndef __rho_app_tWindowGLFW_h__
#define __rho_app_tWindowGLFW_h__


#include <GLFW/glfw3.h>

#include <rho/sync/tThread.h>
#include <rho/sync/tPCQ.h>

#include <string>


namespace rho
{
namespace app
{


// Pre-declarations
class tWindowRunnable;
class tMainLoopGLFW;


/**
 * tWindowGLFW manages a GLFW window in its own thread. All the methods of this
 * class are thread-safe. The constructor and destructor can even be called
 * from secondary threads (such as from other instances of tWindowGLFW).
 *
 * See tMainLoopGLFW for how to handle the application's main thread.
 */
class tWindowGLFW
{
    public:

        /**
         * Creates a normal window using the GLFW3 API. This window
         * is not opened until this object is given to the
         * tMainLoopGLFW object. After the window opens
         * it is managed by its own thread and this->threadMain()
         * is called by that thread.
         *
         * Subclassing notes:
         * Do not call any OpenGL API here, and do not call flipBuffer()
         * or postEvents(). The only thing you should do here is setup
         * your own state.
         */
        tWindowGLFW(u32 width, u32 height, std::string title);

        /**
         * Creates a fullscreen window using the GLFW3 API. This window
         * is not opened until this object is given to the
         * tMainLoopGLFW object. After the window opens
         * it is managed by its own thread and this->threadMain()
         * is called by that thread.
         *
         * Subclassing notes:
         * Do not call any OpenGL API here, and do not call flipBuffer()
         * or postEvents(). The only thing you should do here is setup
         * your own state.
         */
        tWindowGLFW(u8 monitorIndex, std::string title);

        /**
         * Waits for the internal thread to join, then destructs
         * this object.
         *
         * Subclassing notes:
         * Do not call any OpenGL API here, and do not call flipBuffer()
         * or postEvents(). The only thing you should do here is clean-up
         * your own resources.
         */
        virtual ~tWindowGLFW();

        //////////////////////////////////////////////////////////////////////
        //
        // Window management:
        //
        // (Do not override these.)
        //
        //////////////////////////////////////////////////////////////////////

        /**
         * Returns the number of monitors on this computer.
         */
        static u8 numMonitors();

        /**
         * Sets the title of this window.
         */
        void setTitle(std::string newTitle);

        /**
         * Sets the cursor mode of this window.
         *
         * May be:
         *   GLFW_CURSOR_NORMAL makes the cursor visible and behaving normally.
         *   GLFW_CURSOR_HIDDEN makes the cursor invisible when it is over the
         *                      client area of the window.
         *   GLFW_CURSOR_DISABLED disables the cursor and removes any
         *                        limitations on cursor movement.
         *
         * GLFW_CURSOR_NORMAL is default.
         */
        void setCursorMode(int mode);

        /**
         * Gets all the events since last call to this method,
         * and calls whatever callbacks are applicable to those
         * events. This method should be called from threadMain()
         * so that the callbacks are on the same thread as
         * threadMain(). Doing so simplifies the synchronization
         * scheme of state held by the subclasses.
         */
        void postEvents();

        /**
         * Enables monitor refresh synchronization with flipBuffer()
         * so that screen splitting doesn't happen. Enabling this
         * causes flipBuffer() to block for a little bit though, slowing
         * the frame-rate down.
         *
         * Disabled by default.
         */
        void vsyncEnabled(bool enabled);

        /**
         * Flips the front and back render buffers. Should be
         * called from threadMain() after rendering happens.
         */
        void flipBuffer();

        /**
         * Returns true if the window's close button was hit or
         * if setWindowShouldClose() was called.
         */
        bool shouldWindowClose();

        /**
         * Tell the window that it should close on the next loop.
         * Useful if you want the window to close as a result of
         * user input, such as hitting ESCAPE.
         * The window is also told to close if the user hits the
         * window's close button.
         */
        void setWindowShouldClose();

        //////////////////////////////////////////////////////////////////////
        //
        // Receiving callbacks:
        //
        // You must call postEvents() from windowMain() in order to receive
        // these callbacks.
        //
        // (Override only what you need.)
        //
        //////////////////////////////////////////////////////////////////////

        /**
         * Called when the window position changes.
         */
        virtual void windowPosCallback(i32 xpos, i32 ypos);

        /**
         * Called when the window size changes.
         */
        virtual void windowResizedCallback(u32 newWidth, u32 newHeight);

        /**
         * Called when the OpenGL framebuffer's size changes.
         */
        virtual void framebufferResizedCallback(u32 newWidth, u32 newHeight);

        /**
         * Called when a key event happens.
         *
         * All values of 'key' can be seen at:
         *      http://www.glfw.org/docs/latest/group__keys.html
         *
         * 'scancode' is used for keys which have no GLFW key code.
         *
         * 'action' is one of: GLFW_PRESS, GLFW_RELEASE, GLFW_REPEAT
         *
         * 'mods' is a bit field of: GLFW_MOD_SHIFT, GLFW_MOD_CONTROL,
         *                           GLFW_MOD_ALT, GLFW_MOD_SUPER
         */
        virtual void keyCallback(int key, int scancode, int action, int mods);

        /**
         * Called when a unicode character is typed.
         */
        virtual void charCallback(unsigned int unicodeChar);

        /**
         * Called when the mouse either enters or leaves the window.
         */
        virtual void mouseEnteredWindowCallback(bool didEnter);

        /**
         * Called when the mouse pointer moves in the window.
         */
        virtual void mousePosCallback(double xpos, double ypos);

        /**
         * Called when the mouse scroll-wheel is scrolled.
         */
        virtual void mouseScrollCallback(double xoffset, double yoffset);

        /**
         * Called when a mouse button is pressed.
         *
         * 'button' is one of: GLFW_MOUSE_BUTTON_LEFT, GLFW_MOUSE_BUTTON_MIDDLE,
         *                     GLFW_MOUSE_BUTTON_RIGHT, plus some others that
         *                     are not likely to be used.
         *
         * 'action' is one of GLFW_PRESS, GLFW_RELEASE
         *
         * 'mods' is the same as in keyCallback().
         */
        virtual void mouseButtonCallback(int button, int action, int mods);

        //////////////////////////////////////////////////////////////////////
        //
        // This method is called on its own thread. Do not call this method
        // yourself!
        //
        // Override this method and call the OpenGL API all you want.
        //
        // Call postEvents() and flipBuffer() from this method to receive
        // event callbacks and flip the render-buffer.
        //
        //////////////////////////////////////////////////////////////////////

        /**
         * Called on its own thread. Override and call the OpenGL API.
         * Call postEvents() and flipBuffer() periodically from this method.
         *
         * You can create new windows from this method. Simply allocate them
         * and hand them off to the 'mainLoop' object.
         */
        virtual void windowMain(tMainLoopGLFW* mainLoop) = 0;

    private:

        // These are internally used by tMainLoopGLFW.
        void m_open(tMainLoopGLFW* mainLoop);
        void m_setIsDone();
        bool m_isDone();
        void m_close();
        bool m_hasNewTitle(std::string& newTitle);

    private:

        u32 m_initWidth;
        u32 m_initHeight;
        std::string m_initTitle;
        bool m_initFullscreen;
        u8 m_initMonitorIndex;

        tMainLoopGLFW* m_mainLoop;
        GLFWwindow* m_window;
        refc<sync::tThread> m_thread;
        sync::tPCQ< std::vector<u8> > m_eventQueue;

        sync::tMutex m_mux;
        bool m_done;
        std::string m_oldTitle;
        std::string m_newTitle;

        friend class tWindowRunnable;
        friend class tMainLoopGLFW;
};


}   // namespace app
}   // namespace rho


#endif   // __rho_app_tWindowGLFW_h__
