#include <rho/app/tMainLoopGLFW.h>
#include <rho/sync/tThread.h>


namespace rho
{
namespace app
{


tMainLoopGLFW::tMainLoopGLFW()
    : m_newWindowsQueue(8, sync::kGrow)
{
    if (!glfwInit())
        throw eRuntimeError("Cannot initialize GLFW!");
}

tMainLoopGLFW::~tMainLoopGLFW()
{
    loop();
    glfwTerminate();
}

void tMainLoopGLFW::addWindow(refc<tWindowGLFW> window)
{
    m_newWindowsQueue.push(window);
}

u32  tMainLoopGLFW::numWindows()
{
    return (u32)(m_windows.size() + m_newWindowsQueue.size());
}

void tMainLoopGLFW::once()
{
    while (m_newWindowsQueue.size() > 0)
    {
        refc<tWindowGLFW> window = m_newWindowsQueue.pop();
        m_windows.insert(window);
        window->m_open(this);
    }

    bool foundOne = true;
    while (foundOne)
    {
        foundOne = false;
        std::set< refc<tWindowGLFW> >::iterator itr;
        for (itr = m_windows.begin(); itr != m_windows.end(); itr++)
        {
            refc<tWindowGLFW> window = *itr;
            if (window->m_isDone())
            {
                window->m_close();
                m_windows.erase(itr);
                foundOne = true;
                break;
            }
        }
    }

    std::set< refc<tWindowGLFW> >::iterator itr;
    std::string newTitle;
    for (itr = m_windows.begin(); itr != m_windows.end(); itr++)
    {
        refc<tWindowGLFW> window = *itr;
        if (window->m_hasNewTitle(newTitle))
            glfwSetWindowTitle(window->m_window, newTitle.c_str());
    }

    glfwPollEvents();
}

void tMainLoopGLFW::loop()
{
    while (numWindows() > 0)
    {
        once();
        sync::tThread::msleep(10);
    }
}


}   // namespace app
}   // namespace rho
