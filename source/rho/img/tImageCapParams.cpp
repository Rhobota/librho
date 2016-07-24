#include <rho/img/tImageCapParams.h>


namespace rho
{
namespace img
{


tImageCapParams::tImageCapParams()
    :
        deviceIndex(),

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


void tImageCapParams::pack(iWritable* out) const
{
    rho::pack(out, deviceIndex);

    rho::pack(out, inputIndex);
    rho::pack(out, inputDescription);

    rho::pack(out, (u32)displayFormat);
    rho::pack(out, displayFormatDescription);

    rho::pack(out, (u32)captureFormat);
    rho::pack(out, captureFormatDescription);

    rho::pack(out, imageWidth);
    rho::pack(out, imageHeight);

    rho::pack(out, frameIntervalNumerator);
    rho::pack(out, frameIntervalDenominator);
}


void tImageCapParams::unpack(iReadable* in)
{
    rho::unpack(in, deviceIndex);

    rho::unpack(in, inputIndex);
    rho::unpack(in, inputDescription);

    u32 format;
    rho::unpack(in, format);
    displayFormat = (nImageFormat)format;
    rho::unpack(in, displayFormatDescription);

    rho::unpack(in, format);
    captureFormat = (nImageFormat)format;
    rho::unpack(in, captureFormatDescription);

    rho::unpack(in, imageWidth);
    rho::unpack(in, imageHeight);

    rho::unpack(in, frameIntervalNumerator);
    rho::unpack(in, frameIntervalDenominator);
}


std::ostream& operator<< (std::ostream& o, const tImageCapParams& params)
{
    o << "Device #: " << params.deviceIndex << std::endl;
    o << "  Input #" << params.inputIndex << "  (" << params.inputDescription << ")" << std::endl;
    o << "    Display format: " << params.displayFormatDescription << std::endl;
    o << "    Capture format: " << params.captureFormatDescription << std::endl;
    o << "    Frame size:     " << params.imageWidth << " x " << params.imageHeight << std::endl;
    o << "    Frame interval: " << params.frameIntervalNumerator << "/"
                                << params.frameIntervalDenominator << std::endl;
    return o;
}


}    // namespace img
}    // namespace rho
