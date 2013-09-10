#include <rho/app/tMainLoopGLFW.h>
#include <rho/app/tWindowGLFW.h>

#include <iostream>

using namespace rho;
using std::cout;
using std::endl;


class tMyWindowGLFW : public rho::app::tWindowGLFW
{
    public:

        tMyWindowGLFW()
            : rho::app::tWindowGLFW(200, 100, "My Window!")
        {
        }

        void windowMain()
        {
            while (!shouldWindowClose())
            {
                glClearColor(1.0, 0.0, 0.0, 1.0);
                glClear(GL_COLOR_BUFFER_BIT);
                flipBuffer();
                postEvents();
            }
        }

        void windowPosCallback(i32 xpos, i32 ypos)
        {
            cout << "Window pos callback: " << xpos << ", " << ypos << endl;
        }

        void windowResizedCallback(u32 newWidth, u32 newHeight)
        {
            cout << "Window resize callback: " << newWidth << ", " << newHeight << endl;
        }

        void framebufferResizedCallback(u32 newWidth, u32 newHeight)
        {
            cout << "Framebuffer resize callback: " << newWidth << ", " << newHeight << endl;
        }

        void keyCallback(int key, int scancode, int action, int mods)
        {
            cout << "Key callback: " << key << " " << scancode << " " << action << " " << mods << endl;
            if (key == GLFW_KEY_ESCAPE && action == GLFW_RELEASE)
                setWindowShouldClose();
        }

        void charCallback(unsigned int unicodeChar)
        {
            cout << "Char callback: " << unicodeChar << endl;
        }

        void mouseEnteredWindowCallback(bool didEnter)
        {
            cout << "Mouse entered? " << (didEnter ? "yes" : "no") << endl;
        }

        void mousePosCallback(double xpos, double ypos)
        {
            cout << "Mouse pos callback: " << xpos << ", " << ypos << endl;
        }

        void mouseScrollCallback(double xoffset, double yoffset)
        {
            cout << "Mouse scroll callback: " << xoffset << ", " << yoffset << endl;
        }

        void mouseButtonCallback(int button, int action, int mods)
        {
            cout << "Mouse button callback: " << button << " " << action << " " << mods << endl;
        }
};


int main()
{
    rho::app::tMainLoopGLFW mainLoop;

    for (int i = 0; i < 3; i++)
    {
        refc<rho::app::tWindowGLFW> window(new tMyWindowGLFW);
        mainLoop.addWindow(window);
    }

    while (true)
    {
        sync::tThread::msleep(10);
        mainLoop.once();
        if (mainLoop.numWindows() == 0)
            break;
    }

    return 0;
}
