#ifndef __rho_gl_tLight_h__
#define __rho_gl_tLight_h__


#include <rho/geo/tVector.h>


namespace rho
{
namespace gl
{


class tLight
{
    public:

        tLight();

        void enable();
        void disable();

        void setLocation(geo::tVector p);

        void setAmbientColor(geo::tVector c);
        void setDiffuseColor(geo::tVector c);
        void setSpecularColor(geo::tVector c);

    private:

        geo::tVector m_loc;
        geo::tVector m_ambient;
        geo::tVector m_diffuse;
        geo::tVector m_specular;

        int m_index;
};


}    // namespace gl
}    // namespace rho


#endif   // __rho_gl_tLight_h__
