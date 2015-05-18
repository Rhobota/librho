#include <rho/audio/tWaveMaker.h>
#include <rho/tCrashReporter.h>

#include <iostream>

using namespace rho;
using std::cout;
using std::endl;
using std::vector;


int main()
{
    tCrashReporter::init();

    audio::tWaveMaker m = audio::tWaveMaker::readFromFile("example1.wav");

    u32 sampleRate = m.getSampleRate();

    cout << "Wave file has been read!" << endl << endl;
    cout << "Sample rate is " << sampleRate << " Hz." << endl << endl;

    cout << "Will write a wave file which is double that sample rate... ";

    vector<double> wave = m.getWave();

    audio::tWaveMaker m2(wave, 2*sampleRate);

    m2.writeToFile("example2.wav");

    cout << "DONE!" << endl;

    return 0;
}
