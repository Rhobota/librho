#include <rho/img/tVpxImageEncoder.h>
#include <rho/img/tVpxImageAsyncReadable.h>
#include <rho/img/tImageCapFactory.h>
#include <rho/sync/tStreamPCQ.h>
#include <rho/sync/tThread.h>
#include <rho/tCrashReporter.h>
#include <rho/tTest.h>


using namespace rho;


class tReaderThread : public sync::iRunnable,
                      public img::iAsyncReadableImageObserver,
                      public bNonCopyable
{
    public:

        tReaderThread(const tTest& t, refc<iReadable> readable)
            : t(t),
              m_readable(readable),
              m_imageReadable(this),
              m_stop(false),
              m_imageCounter(0)
        {
        }

        void run()
        {
            try
            {
                while (!m_stop)
                {
                    u8 buf[1024];
                    i32 r = m_readable->read(buf, 1024);
                    if (r <= 0)
                        break;
                    m_imageReadable.takeInput(buf, r);
                }
            }
            catch (std::exception& e)
            {
                std::cerr << "Caught exception: " << e.what() << std::endl;
                t.fail();
            }
            m_imageReadable.endStream();
        }

        void gotImage(const img::tImage& image)
        {
            std::cout << "GOT image  " << m_imageCounter++ << std::endl;
        }

        void endStream()
        {
            m_stop = true;
        }

    private:

        const tTest& t;
        refc<iReadable> m_readable;
        img::tVpxImageAsyncReadable m_imageReadable;
        bool m_stop;
        int m_imageCounter;
};


void testDelay(const tTest& t)
{
    refc<img::iImageCapParamsEnumerator> enumerator;

    try
    {
        enumerator =
            img::tImageCapFactory::getImageCapParamsEnumerator();
    }
    catch (eNotImplemented& e)
    {
        return;   // it's ok if the enumerator is not implemented on this platform
    }

    if (enumerator->size() == 0)
        return;

    img::tImageCapParams params = (*enumerator)[0];
    params.displayFormat = img::kRGB24;

    refc<img::iImageCap> cap;

    try
    {
        cap =
            img::tImageCapFactory::getImageCap(params, true);
    }
    catch (eNotImplemented& e)
    {
        return;   // it's ok if the capturer is not implemented on this platform
    }

    img::tImage image;

    refc< sync::tPCQ<std::pair<u8*, u32> > > pcq(new sync::tPCQ<std::pair<u8*, u32> >(8, sync::kGrow));
    refc<iReadable> readable(new sync::tStreamPCQ(pcq, sync::tStreamPCQ::kReadableEnd));
    refc<iWritable> writable(new sync::tStreamPCQ(pcq, sync::tStreamPCQ::kWritableEnd));

    sync::tThread thread(refc<sync::iRunnable>(new tReaderThread(t, readable)));

    img::tVpxImageEncoder encoder(writable, 10000,
                                  params.frameIntervalDenominator / params.frameIntervalNumerator,
                                  params.imageWidth, params.imageHeight);

    for (int i = 0; i < 10; i++)
    {
        cap->getFrame(&image);
        std::cout << "SENT image " << i << std::endl;
        encoder.encodeImage(image);
    }
    encoder.signalEndOfStream();

    thread.join();
}


int main()
{
    tCrashReporter::init();

    tTest("VPX Delay Test", testDelay);

    return 0;
}
