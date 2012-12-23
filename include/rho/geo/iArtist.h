#ifndef __rho_geo_iArtist_h__
#define __rho_geo_iArtist_h__


namespace rho
{
namespace geo
{


class tBox;
class tRect;
class tMesh;


class iArtist
{
    public:

        virtual void drawBox(tBox box) = 0;

        virtual void drawRect(tRect rect) = 0;

        virtual void drawMesh(const tMesh& mesh) = 0;

        virtual ~iArtist() { }
};


}     // namespace geo
}     // namespace rho


#endif    // __rho_geo_iArtist_h__
