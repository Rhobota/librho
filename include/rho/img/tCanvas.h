#ifndef __rho_img_tCanvas_h__
#define __rho_img_tCanvas_h__


#include <rho/img/nImageFormat.h>
#include <rho/img/tImage.h>
#include <rho/geo/tRect.h>

#include <vector>


namespace rho
{
namespace img
{


/**
 * This is a very simple canvas for drawing images onto. It is not optimized
 * and it doesn't have very man features, but it is great for research-related
 * visualizations. Librho is a research library after all.
 */
class tCanvas : public bNonCopyable
{
    public:

        /**
         * Creates a canvas of the specified image format and with
         * the specified background color.
         */
        tCanvas(nImageFormat format, const u8 bgColor[], u32 bgColorSize);

        /**
         * Draws an image onto the canvas. The top-right of the image
         * is placed at (x, y) on the canvas. Note, x and y may be negative.
         */
        void drawImage(const tImage* image, i32 x, i32 y);

        /**
         * If you need the canvas to include a certain point, even
         * if no image is at that point (or beyond that point), call
         * this method. This will force the canvas to expand so that
         * the point is in the final result.
         */
        void expandToIncludePoint(i32 x, i32 y);

        /**
         * Generate the resulting canvas image. This can be called several times, and
         * it can be called mid-way through drawing images, no problem!
         */
        void genImage(tImage* dest);

        /**
         * Resets the canvas to its initial state, as if it was just constructed.
         */
        void reset();

        /**
         * D'tor...
         */
        ~tCanvas();

    private:

        nImageFormat m_format;
        u8*          m_bgColor;
        u32          m_bgColorSize;

        std::vector<tImage*> m_images;
        std::vector<i32>     m_xs;
        std::vector<i32>     m_ys;
        geo::tRect           m_unionArea;
};


}   // namespace img
}   // namespace rho


#endif   // __rho_img_tCanvas_h__
