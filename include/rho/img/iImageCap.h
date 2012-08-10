#ifndef __rho_img_iImageCap_h__
#define __rho_img_iImageCap_h__


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
        virtual  int                     getRequiredBufSize()             const = 0;
        virtual  int                     getFrame(u8* buf, int bufSize)         = 0;
};


}   // namespace img
}   // namespace rho


#endif   // __rho_img_iImageCap_h__
