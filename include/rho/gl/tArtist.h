#ifndef __rho_gl_tArtist_h__
#define __rho_gl_tArtist_h__


#include <rho/iArtist.h>
#include <rho/types.h>

#include <map>
#include <string>


namespace rho
{
namespace gl
{


enum nRenderMode
{
    kFilled,
    kWireframe
};


class tArtist : public iArtist
{
    public:

        tArtist(nRenderMode rm = kFilled);
        ~tArtist();

        void draw(const iDrawable& drawable);

        void setTexture(const std::string& textureFilePath);
        void unsetTexture();

    private:

        nRenderMode m_rm;

        std::map<std::string, u32> m_textureFileMap;
};


}     // namespace gl
}     // namespace rho


#endif    // __rho_gl_tArtist_h__
