#include <rho/img/tImageCapParams.h>


namespace rho
{
namespace img
{


tImageCapParams::tImageCapParams()
    :
        deviceURL(),

        inputIndex(0),
        inputDescription(),

        displayFormat(kUnspecified),
        displayFormatDescription(),

        captureFormat(kUnspecified),
        captureFormatDescription(),

        imageWidth(320),
        imageHeight(240),

        frameIntervalNumerator(1),
        frameIntervalDenominator(30)
{
}


std::ostream& operator<< (std::ostream& o, const tImageCapParams& params)
{
    o << "Device URL: " << params.deviceURL << std::endl;
    o << "  Input #" << params.inputIndex << "  (" << params.inputDescription << ")" << std::endl;
    o << "    Capture format: " << params.captureFormatDescription << std::endl;
    o << "    Frame size:     " << params.imageWidth << " x " << params.imageHeight << std::endl;
    o << "    Frame interval: " << params.frameIntervalNumerator << "/"
                                << params.frameIntervalDenominator << std::endl;
    return o;
}


}    // namespace img
}    // namespace rho
