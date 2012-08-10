#include <rho/audio/tWaveMaker.h>
#include <rho/tCrashReporter.h>

#include <iostream>

using namespace rho;
using std::cout;
using std::endl;


int main()
{
    tCrashReporter::init();

    u32 sampleRate = 8000;
    u32 numSamples = sampleRate * 3.5;

    audio::tWaveMaker m(numSamples, sampleRate);

    {
        double frequency = 1000.0;
        double amplitute = 1.0;
        m.addWave(frequency, amplitute);
    }

    {
        double frequency = 400.0;
        double amplitute = 0.5;
        m.addWave(frequency, amplitute);
    }

    {
        double frequency = 123.0;
        double amplitute = 0.8;
        m.addWave(frequency, amplitute);
    }

    m.writeToFile("example1.wav");

    cout << "Wave file has been written!" << endl;

    return 0;
}
