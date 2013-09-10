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
            m_window->windowMain();
            m_window->m_done = true;
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

tWindowGLFW::tWindowGLFW(u32 width, u32 height, string title,
            bool fullscreen, u8 monitorIndex)
    : m_initWidth(width), m_initHeight(height),
      m_initTitle(title), m_initFullscreen(fullscreen),
      m_initMonitorIndex(monitorIndex), m_window(NULL),
      m_thread(), m_eventQueue(8, sync::kGrow),
      m_done(false)
{
}

tWindowGLFW::~tWindowGLFW()
{
    if (m_thread)
    {
        m_thread->join();
        m_thread = NULL;
    }
}

void tWindowGLFW::setTitle(string newTitle)
{
    // todo -- glfwSetWindowTitle() can only be called from the main thread
}

void tWindowGLFW::setCursorMode(int mode)
{
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
    glfwSwapBuffers(m_window);
}

bool tWindowGLFW::shouldWindowClose()
{
    return glfwWindowShouldClose(m_window) != 0;
}

void tWindowGLFW::setWindowShouldClose()
{
    glfwSetWindowShouldClose(m_window, 1);
}

void tWindowGLFW::windowPosCallback(i32 xpos, i32 ypos)
{
}

void tWindowGLFW::windowResizedCallback(u32 newWidth, u32 newHeight)
{
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

void tWindowGLFW::m_open()
{
    if (m_initFullscreen)
    {
        int count = 0;
        GLFWmonitor** monitors = glfwGetMonitors(&count);
        if (m_initMonitorIndex >= count)
            throw eInvalidArgument("No monitor with that index.");
        m_window = glfwCreateWindow((int)m_initWidth, (int)m_initHeight, m_initTitle.c_str(),
                                     monitors[m_initMonitorIndex], NULL);
    }
    else
    {
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

bool tWindowGLFW::m_isDone()
{
    return m_done;
}

void tWindowGLFW::m_close()
{
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

    glfwDestroyWindow(m_window);
    m_window = NULL;
}


}  // namespace app
}  // namespace rho
