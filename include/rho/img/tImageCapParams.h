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

        int          deviceIndex;

        int          inputIndex;
        std::string  inputDescription;

        nImageFormat displayFormat;
        std::string  displayFormatDescription;

        nImageFormat captureFormat;
        std::string  captureFormatDescription;

        int          imageWidth;
        int          imageHeight;

        int          frameIntervalNumerator;      // seconds per frame
        int          frameIntervalDenominator;    //
};


std::ostream& operator<< (std::ostream& o, const tImageCapParams& params);


}    // namespace img
}    // namespace rho


#endif   // __rho_img_tImageCapParams_h__
