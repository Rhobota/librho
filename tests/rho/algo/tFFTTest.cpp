#include <rho/algo/tFFT.h>
#include <rho/audio/tWaveMaker.h>
#include <rho/tTest.h>
#include <rho/tCrashReporter.h>

#include <cstdlib>
#include <ctime>

using namespace rho;
using std::vector;


static const int kNumTests = 100;


vector<double> createRandomSignal(u32 numSamples, u32 sampleRate)
{
    audio::tWaveMaker m(numSamples, sampleRate);

    int numPartials = (rand() % 20) + 1;

    for (int i = 0; i < numPartials; i++)
    {
        double freq = rand() / RAND_MAX * 1000.0;
        double ampl = rand() / RAND_MAX * 3.0;
        m.addWave(freq, ampl);
    }

    return m.getWave();
}


vector<double> createRandomSignalForFFT(u32 sampleRate)
{
    u32 numSamples = 1 << (rand() % 14);
    return createRandomSignal(numSamples, sampleRate);
}


vector<double> createRandomSignalForDFT(u32 sampleRate)
{
    u32 numSamples = (rand() % 200) + 1;
    return createRandomSignal(numSamples, sampleRate);
}


void fftTest(const tTest& t)
{
    u32 sampleRate = 8000;

    vector<double> signal = createRandomSignalForFFT(sampleRate);

    algo::tFFT fft(signal, sampleRate);

    vector<double> real = fft.getRealPart();
    vector<double> img  = fft.getImaginaryPart();

    vector<double> reconstructedSignal =
        algo::tFFT::inverse(real, img);

    t.iseq(signal, reconstructedSignal);
}


void dftTest(const tTest& t)
{
    u32 sampleRate = 8000;

    vector<double> signal = createRandomSignalForDFT(sampleRate);

    algo::tDFT dft(signal, sampleRate);

    vector<double> real = dft.getRealPart();
    vector<double> img  = dft.getImaginaryPart();

    vector<double> reconstructedSignal =
        algo::tDFT::inverse(real, img);

    t.iseq(signal, reconstructedSignal);
}


int main()
{
    tCrashReporter::init();

    srand((u32)time(0));

    tTest("tFFT test", fftTest, kNumTests);
    tTest("tDFT test", dftTest, kNumTests);

    return 0;
}
