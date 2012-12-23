#ifndef __rho_gl_tArtist_h__
#define __rho_gl_tArtist_h__


#include <rho/geo/iArtist.h>


namespace rho
{
namespace gl
{


enum nRenderMode
{
    kFilled,
    kWireframe
};


class tArtist : public geo::iArtist
{
    public:

        tArtist(nRenderMode rm = kFilled);

        void drawBox(geo::tBox b);
        void drawRect(geo::tRect r);
        void drawMesh(const geo::tMesh& mesh);

    private:

        nRenderMode m_rm;
};


}     // namespace gl
}     // namespace rho


#endif    // __rho_gl_tArtist_h__
