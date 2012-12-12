#ifndef __rho_img_iImageCapParamsEnumerator_h__
#define __rho_img_iImageCapParamsEnumerator_h__


#include <rho/img/tImageCapParams.h>


namespace rho
{
namespace img
{


class iImageCapParamsEnumerator
{
    public:

        virtual int size() const = 0;

        virtual const tImageCapParams& operator[] (int i) = 0;

        virtual ~iImageCapParamsEnumerator() { }
};


}    // namespace img
}    // namespace rho


#endif   // __rho_img_iImageCapParamsEnumerator_h__
