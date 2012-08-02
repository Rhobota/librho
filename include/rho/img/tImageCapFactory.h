#ifndef __rho_img_tImageCapFactory_h__
#define __rho_img_tImageCapFactory_h__


#include <rho/img/iImageCap.h>
#include <rho/img/iImageCapParamsEnumerator.h>

#include <rho/bNonCopyable.h>
#include <rho/refc.h>


namespace rho
{
namespace img
{


class tImageCapFactory : public bNonCopyable
{
    public:

        static
        refc<iImageCapParamsEnumerator> getImageCapParamsEnumerator();

        static
        refc<iImageCap> getImageCap(
                tImageCapParams& params,
                bool allowCoercion
        );

    private:

        tImageCapFactory();
};


}    // namespace img
}    // namespace rho


#endif    // __rho_img_tImageCapFactory_h__
