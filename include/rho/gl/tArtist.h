#ifndef __rho_gl_tArtist_h__
#define __rho_gl_tArtist_h__


#include <rho/iArtist.h>


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

        void draw(const iDrawable& drawable);

    private:

        nRenderMode m_rm;
};


}     // namespace gl
}     // namespace rho


#endif    // __rho_gl_tArtist_h__
