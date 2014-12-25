#include <rho/algo/tFFT.h>
#include <rho/audio/tWaveMaker.h>
#include <rho/tCrashReporter.h>

#include <iostream>

using namespace rho;
using std::vector;
using std::pair;
using std::cout;
using std::endl;


int main()
{
    tCrashReporter::init();

    u32 sampleRate = 8000;
    u32 numSamples = 1 << 14;

    audio::tWaveMaker m(numSamples, sampleRate);

    {
        double freq = 1000.0;
        double amp  = 4.2;
        m.addWave(freq, amp);
    }

    {
        double freq = 325.0;
        double amp  = 2.8;
        m.addWave(freq, amp);
    }

    {
        double freq = 670.0;
        double amp  = 1.5;
        m.addWave(freq, amp);
    }

    vector<double> signal = m.getWave();

    algo::tFFT fft(signal, sampleRate);

    vector< pair<double, double> > ampFreqPairs =
        fft.getAmpFreqPairs();

    cout << "Printing the five partials with the largest amplitudes ..." << endl << endl;
    cout << "Frequency   (amplitude)" << endl;
    for (int i = 0; i < 5; i++)
    {
        int k = ampFreqPairs.size() - 1 - i;
        cout << ampFreqPairs[k].second << "   ("
             << ampFreqPairs[k].first << ")" << endl;
    }

    return 0;
}
