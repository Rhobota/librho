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

    int bufSize = cap->getRequiredBufSize();
    t.assert(bufSize > 0);

    u8* buf = new u8[bufSize];

    for (int i = 0; i < 20; i++)
    {
        int readSize = cap->getFrame(buf, bufSize);
        t.assert(readSize > 0);
    }

    delete [] buf;
}


int main()
{
    tCrashReporter::init();

    tTest("Params enumeration test", testParamsEnumerator);
    tTest("Image capture test", testImageCap);

    return 0;
}
