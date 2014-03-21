#ifndef __rho_app_tSimpleImageWindow_h__
#define __rho_app_tSimpleImageWindow_h__


#include <rho/app/tWindowGLFW.h>
#include <rho/img/tImage.h>
#include <rho/sync/tAutoSync.h>
#include <rho/sync/tMutex.h>


namespace rho
{
namespace app
{


/**
 * Creates a window whose job is to display whatever image you give it.
 *
 * The window will close if escape is pressed on it.
 */
class tSimpleImageWindow : public tWindowGLFW
{
    public:

        tSimpleImageWindow(u32 initWidth, u32 initHeight, std::string initTitle);
        ~tSimpleImageWindow();

        void setImage(img::tImage* image);

    public:

        // tWindowGLFW callbacks (just the ones we need):
        void windowResizedCallback(u32 newWidth, u32 newHeight);
        void keyCallback(int key, int scancode, int action, int mods);

        // The tWindowGLFW window main method:
        void windowMain(tMainLoopGLFW* mainLoop);

    private:

        sync::tMutex m_mux;
        img::tImage* m_image;
};


}   // namespace app
}   // namespace rho


#endif  // __rho_app_tSimpleImageWindow_h__
