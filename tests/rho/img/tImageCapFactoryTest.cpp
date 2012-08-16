#include <rho/img/tImageCapFactory.h>
#include <rho/tCrashReporter.h>
#include <rho/tTest.h>


using namespace rho;


void testParamsEnumerator(const tTest& t)
{
    refc<img::iImageCapParamsEnumerator> enumerator =
        img::tImageCapFactory::getImageCapParamsEnumerator();

    t.assert(enumerator->size() > 0);

    for (int i = 0; i < enumerator->size(); i++)
    {
        img::tImageCapParams params = (*enumerator)[i];
    }
}


void testImageCap(const tTest& t)
{
    refc<img::iImageCapParamsEnumerator> enumerator =
        img::tImageCapFactory::getImageCapParamsEnumerator();

    img::tImageCapParams params = (*enumerator)[0];
    params.displayFormat = img::kRGB24;

    refc<img::iImageCap> cap =
        img::tImageCapFactory::getImageCap(params, true);

    img::tImageCapParams coercedParams = cap->getParams();

    t.assert(params.deviceIndex == coercedParams.deviceIndex);
    t.assert(params.inputIndex == coercedParams.inputIndex);
    t.assert(params.inputDescription == coercedParams.inputDescription);
    t.assert(params.captureFormat == coercedParams.captureFormat);
    t.assert(params.captureFormatDescription == coercedParams.captureFormatDescription);
    t.assert(params.displayFormat == coercedParams.displayFormat);
    t.assert(params.displayFormatDescription == coercedParams.displayFormatDescription);
    t.assert(params.imageWidth == coercedParams.imageWidth);
    t.assert(params.imageHeight == coercedParams.imageHeight);
    t.assert(params.frameIntervalNumerator == coercedParams.frameIntervalNumerator);
    t.assert(params.frameIntervalDenominator == coercedParams.frameIntervalDenominator);

    u32 bufSize = cap->getRequiredBufSize();
    t.assert(bufSize > 0);

    img::tImage image;

    for (int i = 0; i < 20; i++)
    {
        cap->getFrame(&image);

        t.assert(image.buf() != NULL);
        t.assert(image.bufSize() == bufSize);
        t.assert(image.bufUsed() > 0);
        t.assert(image.width() == params.imageWidth);
        t.assert(image.height() == params.imageHeight);
        t.assert(image.format() == params.displayFormat);
    }
}


int main()
{
    tCrashReporter::init();

    tTest("Params enumeration test", testParamsEnumerator);
    tTest("Image capture test", testImageCap);

    return 0;
}
