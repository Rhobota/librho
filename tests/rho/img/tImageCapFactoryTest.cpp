#include <rho/img/tImageCapFactory.h>

#include <rho/ip/tcp/tSocket.h>

#include <signal.h>

#include <iostream>


using namespace rho;
using std::cout;
using std::endl;


bool gStop = false;

static ip::tcp::tSocket gViewerSocket("::1", 12345);


void gotKeyboardInterrupt(int sig)
{
    if (sig == SIGINT)
    {
        gStop = true;
    }
}


void testParamsEnumerator()
{
    refc<img::iImageCapParamsEnumerator> enumerator =
        img::tImageCapFactory::getImageCapParamsEnumerator();

    cout << "Size: " << enumerator->size() << endl;

    for (int i = 0; i < enumerator->size(); i++)
    {
        cout << (*enumerator)[i] << endl;
    }
}


void testImageCap()
{
    img::tImageCapParams params;
    params.deviceURL = "/dev/video0";
    params.captureFormat = img::kYUYV;
    params.displayFormat = img::kRGB24;

    cout << params << endl;

    refc<img::iImageCap> cap =
        img::tImageCapFactory::getImageCap(params, false);

    int bufSize = cap->getRequiredBufSize();
    u8* buffer = new u8[bufSize];

    while (!gStop)
    {
        int size = cap->getFrame(buffer, bufSize);
        gViewerSocket.write(buffer, size);
        cout << "Read a frame, size: " << size << endl;
    }
}


int main()
{
    signal(SIGINT, gotKeyboardInterrupt);

    testParamsEnumerator();

    testImageCap();

    cout << "Exiting cleanly..." << endl;

    return 0;
}
