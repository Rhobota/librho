#ifndef __rho_img_tImageCapParams_h__
#define __rho_img_tImageCapParams_h__


#include <rho/img/nImageFormat.h>

#include <ostream>
#include <string>


namespace rho
{
namespace img
{


class tImageCapParams
{
    public:

        tImageCapParams();

    public:

        u32          deviceIndex;

        u32          inputIndex;
        std::string  inputDescription;

        nImageFormat displayFormat;
        std::string  displayFormatDescription;

        nImageFormat captureFormat;
        std::string  captureFormatDescription;

        u32          imageWidth;
        u32          imageHeight;

        u32          frameIntervalNumerator;      // seconds per frame
        u32          frameIntervalDenominator;    //
};


std::ostream& operator<< (std::ostream& o, const tImageCapParams& params);


}    // namespace img
}    // namespace rho


#endif   // __rho_img_tImageCapParams_h__
