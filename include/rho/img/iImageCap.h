#ifndef __rho_img_iImageCap_h__
#define __rho_img_iImageCap_h__


#include <rho/ppcheck.h>
#include <rho/img/tImage.h>
#include <rho/img/tImageCapParams.h>

#include <rho/types.h>


namespace rho
{
namespace img
{


class iImageCap
{
    public:

        virtual  const tImageCapParams&  getParams()                      const = 0;
        virtual  u32                     getRequiredBufSize()             const = 0;
        virtual  void                    getFrame(tImage* image)                = 0;

        virtual ~iImageCap() { }
};


}   // namespace img
}   // namespace rho


#endif   // __rho_img_iImageCap_h__
