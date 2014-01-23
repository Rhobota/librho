#ifndef __rho_iDrawable_h__
#define __rho_iDrawable_h__


#include <rho/iArtist.h>
#include <rho/refc.h>


namespace rho
{


class iDrawable
{
    public:

        /**
         * Tells the receiving object to draw itself.
         */
        virtual void draw() = 0;

        virtual ~iDrawable() { }
};


class bDrawableByArtist : public iDrawable
{
    public:

        void setArtist(refc<iArtist> artist)
        {
            m_artist = artist;
        }

        void draw()
        {
            m_artist->draw(*this);
        }

    private:

        refc<iArtist> m_artist;
};


}     // namespace rho


#endif    // __rho_iDrawable_h__
