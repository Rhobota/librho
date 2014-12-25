#ifndef __rho_iArtist_h__
#define __rho_iArtist_h__


#include <rho/ppcheck.h>
#include <rho/eRho.h>


namespace rho
{


class iDrawable;


class iArtist
{
    public:

        /**
         * Draws the given object, or throws eInvalidArgument
         * if the receiver cannot draw the given object.
         */
        virtual void draw(const iDrawable& drawable) = 0;

        virtual ~iArtist() { }
};


}     // namespace rho


#endif    // __rho_iArtist_h__
