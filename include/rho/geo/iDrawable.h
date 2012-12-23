#ifndef __rho_geo_iDrawable_h__
#define __rho_geo_iDrawable_h__


#include <rho/geo/iArtist.h>


namespace rho
{
namespace geo
{


class iDrawable
{
    public:

        virtual void drawWithArtist(iArtist& artist) = 0;

        virtual ~iDrawable() { }
};


}     // namespace geo
}     // namespace rho


#endif    // __rho_geo_iDrawable_h__
