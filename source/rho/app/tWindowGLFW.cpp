#include <rho/app/tWindowGLFW.h>
#include <rho/iReadable.h>
#include <rho/iWritable.h>
#include <rho/iPackable.h>

#include <map>

using namespace std;


namespace rho
{
namespace app
{


static map< GLFWwindow*, sync::tPCQ< vector<u8> >* > gWindowMap;

static
void s_mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    sync::tPCQ< vector<u8> >* q = gWindowMap[window];
    tByteWritable w;
    pack(&w, (u32)0);
    pack(&w, (i32)button);
    pack(&w, (i32)action);
    pack(&w, (i32)mods);
    q->push(w.getBuf());
}

static
void s_mousePosCallback(GLFWwindow* window, double xpos, double ypos)
{
    sync::tPCQ< vector<u8> >* q = gWindowMap[window];
    tByteWritable w;
    pack(&w, (u32)1);
    pack(&w, (f64)xpos);
    pack(&w, (f64)ypos);
    q->push(w.getBuf());
}

static
void s_mouseEnterCallback(GLFWwindow* window, int entered)
{
    sync::tPCQ< vector<u8> >* q = gWindowMap[window];
    u8 didEnter = (entered == GL_TRUE) ? 1 : 0;
    tByteWritable w;
    pack(&w, (u32)2);
    pack(&w, didEnter);
    q->push(w.getBuf());
}

static
void s_mouseScrollCallback(GLFWwindow* window, double xoff, double yoff)
{
    sync::tPCQ< vector<u8> >* q = gWindowMap[window];
    tByteWritable w;
    pack(&w, (u32)3);
    pack(&w, (f64)xoff);
    pack(&w, (f64)yoff);
    q->push(w.getBuf());
}

static
void s_keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    sync::tPCQ< vector<u8> >* q = gWindowMap[window];
    tByteWritable w;
    pack(&w, (u32)4);
    pack(&w, (i32)key);
    pack(&w, (i32)scancode);
    pack(&w, (i32)action);
    pack(&w, (i32)mods);
    q->push(w.getBuf());
}

static
void s_charCallback(GLFWwindow* window, unsigned int character)
{
    sync::tPCQ< vector<u8> >* q = gWindowMap[window];
    tByteWritable w;
    pack(&w, (u32)5);
    pack(&w, (u32)character);
    q->push(w.getBuf());
}

static
void s_windowResized(GLFWwindow* window, int width, int height)
{
    sync::tPCQ< vector<u8> >* q = gWindowMap[window];
    tByteWritable w;
    pack(&w, (u32)6);
    pack(&w, (i32)width);
    pack(&w, (i32)height);
    q->push(w.getBuf());
}

static
void s_framebufferResized(GLFWwindow* window, int width, int height)
{
    sync::tPCQ< vector<u8> >* q = gWindowMap[window];
    tByteWritable w;
    pack(&w, (u32)7);
    pack(&w, (i32)width);
    pack(&w, (i32)height);
    q->push(w.getBuf());
}

static
void s_windowPosCallback(GLFWwindow* window, int xpos, int ypos)
{
    sync::tPCQ< vector<u8> >* q = gWindowMap[window];
    tByteWritable w;
    pack(&w, (u32)8);
    pack(&w, (i32)xpos);
    pack(&w, (i32)ypos);
    q->push(w.getBuf());
}


class tWindowRunnable : public sync::iRunnable
{
    public:

        tWindowRunnable(tWindowGLFW* window)
            : m_window(window)
        {
        }

        void run()
        {
            glfwMakeContextCurrent(m_window->m_window);
            m_window->windowMain(m_window->m_mainLoop);
            m_window->m_setIsDone();
        }

    private:

        tWindowGLFW* m_window;
};


u8 tWindowGLFW::numMonitors()
{
    int count = 0;
    //GLFWmonitor** monitors =
    glfwGetMonitors(&count);
    return (u8)count;
}

tWindowGLFW::tWindowGLFW(u32 width, u32 height, string title)
    : m_initWidth(width), m_initHeight(height),
      m_desiredWidth(width), m_desiredHeight(height), m_newSize(false),
      m_currWidth(width), m_currHeight(height),
      m_initTitle(title), m_initFullscreen(false),
      m_initMonitorIndex(0),
      m_mainLoop(NULL), m_window(NULL),
      m_thread(), m_eventQueue(8, sync::kGrow),
      m_mux(), m_done(false),
      m_oldTitle(title), m_newTitle(title)
{
}

tWindowGLFW::tWindowGLFW(u8 monitorIndex, string title)
    : m_initWidth(0), m_initHeight(0),
      m_desiredWidth(0), m_desiredHeight(0), m_newSize(false),
      m_currWidth(0), m_currHeight(0),
      m_initTitle(title), m_initFullscreen(true),
      m_initMonitorIndex(monitorIndex),
      m_mainLoop(NULL), m_window(NULL),
      m_thread(), m_eventQueue(8, sync::kGrow),
      m_mux(), m_done(false),
      m_oldTitle(title), m_newTitle(title)
{
}

tWindowGLFW::~tWindowGLFW()
{
}

void tWindowGLFW::setTitle(string newTitle)
{
    sync::tAutoSync as(m_mux);
    m_newTitle = newTitle;
}

bool tWindowGLFW::m_hasNewTitle(string& newTitle)
{
    sync::tAutoSync as(m_mux);
    if (m_oldTitle != m_newTitle)
    {
        newTitle = m_newTitle;
        m_oldTitle = m_newTitle;
        return true;
    }
    return false;
}

void tWindowGLFW::setWindowSize(u32 width, u32 height)
{
    sync::tAutoSync as(m_mux);
    if (width != m_desiredWidth || height != m_desiredHeight)
    {
        m_desiredWidth = width;
        m_desiredHeight = height;
        m_newSize = true;
    }
}

bool tWindowGLFW::m_hasNewSize(u32& width, u32& height)
{
    sync::tAutoSync as(m_mux);
    width = m_desiredWidth;
    height = m_desiredHeight;
    bool newSize = m_newSize;
    m_newSize = false;
    return newSize;
}

u32 tWindowGLFW::currWindowWidth() const
{
    sync::tAutoSync as(m_mux);
    return m_currWidth;
}

u32 tWindowGLFW::currWindowHeight() const
{
    sync::tAutoSync as(m_mux);
    return m_currHeight;
}

void tWindowGLFW::setCursorMode(int mode)
{
    sync::tAutoSync as(m_mux);
    if (m_window)
        glfwSetInputMode(m_window, GLFW_CURSOR, mode);
}

void tWindowGLFW::vsyncEnabled(bool enabled)
{
    glfwSwapInterval(enabled ? 1 : 0);
}

void tWindowGLFW::postEvents()
{
    while (m_eventQueue.size() > 0)
    {
        vector<u8> event = m_eventQueue.pop();
        tByteReadable r(event);
        u32 type; unpack(&r, type);
        switch (type)
        {
            case 0:
            {
                i32 button, action, mods;
                unpack(&r, button);
                unpack(&r, action);
                unpack(&r, mods);
                mouseButtonCallback((int)button, (int)action, (int)mods);
                break;
            }

            case 1:
            {
                f64 xpos, ypos;
                unpack(&r, xpos);
                unpack(&r, ypos);
                mousePosCallback((double)xpos, (double)ypos);
                break;
            }

            case 2:
            {
                u8 didEnter;
                unpack(&r, didEnter);
                mouseEnteredWindowCallback(didEnter == 1);
                break;
            }

            case 3:
            {
                f64 xoff, yoff;
                unpack(&r, xoff);
                unpack(&r, yoff);
                mouseScrollCallback((double)xoff, (double)yoff);
                break;
            }

            case 4:
            {
                i32 key, scancode, action, mods;
                unpack(&r, key);
                unpack(&r, scancode);
                unpack(&r, action);
                unpack(&r, mods);
                keyCallback((int)key, (int)scancode, (int)action, (int)mods);
                break;
            }

            case 5:
            {
                u32 character;
                unpack(&r, character);
                charCallback((unsigned int)character);
                break;
            }

            case 6:
            {
                i32 width, height;
                unpack(&r, width);
                unpack(&r, height);
                windowResizedCallback((int)width, (int)height);
                break;
            }

            case 7:
            {
                i32 width, height;
                unpack(&r, width);
                unpack(&r, height);
                framebufferResizedCallback((int)width, (int)height);
                break;
            }

            case 8:
            {
                i32 xpos, ypos;
                unpack(&r, xpos);
                unpack(&r, ypos);
                windowPosCallback((int)xpos, (int)ypos);
                break;
            }

            default:
                throw eLogicError("What happened!?");
        }
    }
}

void tWindowGLFW::flipBuffer()
{
    sync::tAutoSync as(m_mux);
    if (m_window)
        glfwSwapBuffers(m_window);
}

bool tWindowGLFW::shouldWindowClose()
{
    sync::tAutoSync as(m_mux);
    if (m_window)
        return glfwWindowShouldClose(m_window) != 0;
    else
        return true;
}

void tWindowGLFW::setWindowShouldClose()
{
    sync::tAutoSync as(m_mux);
    if (m_window)
        glfwSetWindowShouldClose(m_window, 1);
}

void tWindowGLFW::windowPosCallback(i32 xpos, i32 ypos)
{
}

void tWindowGLFW::windowResizedCallback(u32 newWidth, u32 newHeight)
{
    sync::tAutoSync as(m_mux);
    m_currWidth = newWidth;
    m_currHeight = newHeight;
    m_desiredWidth = newWidth;
    m_desiredHeight = newHeight;
}

void tWindowGLFW::framebufferResizedCallback(u32 newWidth, u32 newHeight)
{
    glViewport(0, 0, newWidth, newHeight);
}

void tWindowGLFW::keyCallback(int key, int scancode, int action, int mods)
{
}

void tWindowGLFW::charCallback(unsigned int unicodeChar)
{
}

void tWindowGLFW::mouseEnteredWindowCallback(bool didEnter)
{
}

void tWindowGLFW::mousePosCallback(double xpos, double ypos)
{
}

void tWindowGLFW::mouseScrollCallback(double xoffset, double yoffset)
{
}

void tWindowGLFW::mouseButtonCallback(int button, int action, int mods)
{
}

void tWindowGLFW::m_open(tMainLoopGLFW* mainLoop)
{
    m_mainLoop = mainLoop;

    if (m_initFullscreen)
    {
        int count = 0;
        GLFWmonitor** monitors = glfwGetMonitors(&count);
        if (m_initMonitorIndex >= count)
            throw eInvalidArgument("No monitor with that index.");
        GLFWmonitor* monitor = monitors[m_initMonitorIndex];
        const GLFWvidmode* vidmode = glfwGetVideoMode(monitor);
        m_initWidth = (u32)vidmode->width;
        m_initHeight = (u32)vidmode->height;
        m_desiredWidth = m_initWidth;
        m_desiredHeight = m_initHeight;
        m_currWidth = m_initWidth;
        m_currHeight = m_initHeight;
        sync::tAutoSync as(m_mux);
        m_window = glfwCreateWindow((int)m_initWidth, (int)m_initHeight, m_initTitle.c_str(),
                                     monitor, NULL);
    }
    else
    {
        sync::tAutoSync as(m_mux);
        m_window = glfwCreateWindow((int)m_initWidth, (int)m_initHeight, m_initTitle.c_str(),
                                     NULL, NULL);
    }

    if (!m_window)
        throw eInvalidArgument("Cannot create GLFW window.");

    gWindowMap[m_window] = &m_eventQueue;

    glfwSetFramebufferSizeCallback(m_window, s_framebufferResized);
    glfwSetWindowSizeCallback(m_window, s_windowResized);
    glfwSetWindowPosCallback(m_window, s_windowPosCallback);
    glfwSetMouseButtonCallback(m_window, s_mouseButtonCallback);
    glfwSetScrollCallback(m_window, s_mouseScrollCallback);
    glfwSetCursorEnterCallback(m_window, s_mouseEnterCallback);
    glfwSetCursorPosCallback(m_window, s_mousePosCallback);
    glfwSetKeyCallback(m_window, s_keyCallback);
    glfwSetCharCallback(m_window, s_charCallback);

    vsyncEnabled(false);

    m_thread = new sync::tThread(refc<sync::iRunnable>(new tWindowRunnable(this)));
}

void tWindowGLFW::m_setIsDone()
{
    sync::tAutoSync as(m_mux);   // I don't really think I need this, but I'm paranoid
    m_done = true;
}

bool tWindowGLFW::m_isDone()
{
    sync::tAutoSync as(m_mux);   // I don't really think I need this, but I'm paranoid
    return m_done;
}

void tWindowGLFW::m_close()
{
    m_thread->join();
    m_thread = NULL;

    glfwSetFramebufferSizeCallback(m_window, NULL);
    glfwSetWindowSizeCallback(m_window, NULL);
    glfwSetWindowPosCallback(m_window, NULL);
    glfwSetMouseButtonCallback(m_window, NULL);
    glfwSetScrollCallback(m_window, NULL);
    glfwSetCursorEnterCallback(m_window, NULL);
    glfwSetCursorPosCallback(m_window, NULL);
    glfwSetKeyCallback(m_window, NULL);
    glfwSetCharCallback(m_window, NULL);

    gWindowMap.erase(m_window);

    {
        sync::tAutoSync as(m_mux);
        glfwDestroyWindow(m_window);
        m_window = NULL;
    }

    m_mainLoop = NULL;
}


}  // namespace app
}  // namespace rho
