#include <rho/app/tSimpleImageWindow.h>

#include <rho/gl/init.h>


namespace rho
{
namespace app
{


tSimpleImageWindow::tSimpleImageWindow(u32 initWidth, u32 initHeight, std::string initTitle)
    : tWindowGLFW(initWidth, initHeight, initTitle),
      m_mux(),
      m_image(NULL)
{
}

tSimpleImageWindow::~tSimpleImageWindow()
{
    delete m_image;
    m_image = NULL;
}

void tSimpleImageWindow::setImage(img::tImage* image)
{
    sync::tAutoSync as(m_mux);
    if (image)
    {
        if (!m_image)
            m_image = new img::tImage;
        image->copyTo(m_image);
        setWindowSize(m_image->width(), m_image->height());
    }
    else
    {
        delete m_image;
        m_image = NULL;
        setWindowSize(300, 100);
    }
}

void tSimpleImageWindow::windowResizedCallback(u32 newWidth, u32 newHeight)
{
    tWindowGLFW::windowResizedCallback(newWidth, newHeight);

    gl::init2d(newWidth, newHeight);
}

void tSimpleImageWindow::keyCallback(int key, int scancode, int action, int mods)
{
    tWindowGLFW::keyCallback(key, scancode, action, mods);

    if (key == GLFW_KEY_ESCAPE && action == GLFW_RELEASE)
        setWindowShouldClose();
}

void tSimpleImageWindow::windowMain(tMainLoopGLFW* mainLoop)
{
    gl::init2d(currWindowWidth(), currWindowHeight());

    while (!shouldWindowClose())
    {
        glClear(GL_COLOR_BUFFER_BIT);

        {
            sync::tAutoSync as(m_mux);
            if (m_image != NULL && m_image->bufUsed() > 0)
                gl::drawImage2d(0, 0, m_image);
        }

        flipBuffer();

        postEvents();

        sync::tThread::msleep(10);
    }
}


}   // namespace app
}   // namespace rho
