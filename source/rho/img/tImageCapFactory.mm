#include <rho/img/tImageCapFactory.h>


#if __linux__
#include "linuxImpl.ipp"
#elif __APPLE__
#include "osxImpl.ipp"
#endif


namespace rho
{
namespace img
{


refc<iImageCapParamsEnumerator> tImageCapFactory::getImageCapParamsEnumerator()
{
    return new tImageCapParamsEnumerator();
}

refc<iImageCap> tImageCapFactory::getImageCap(
        const tImageCapParams& params,
        bool allowCoercion)
{
    return new tImageCap(params);
}


}    // nanespace img
}    // namespace rho
