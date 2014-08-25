#include <rho/audio/tWaveMaker.h>
#include <rho/tTest.h>
#include <rho/tCrashReporter.h>

#include <string>
#include <vector>

using namespace rho;


#if __linux__ || __APPLE__ || __CYGWIN__
std::string gWavePath = "/tmp/wavemakertest.wav";
#elif __MINGW32__
std::string gWavePath = "C:\\wavemakertest.wav";
#else
#error What platform are you on!?
#endif

std::vector<double> gWave;


std::vector<double> normalize(const std::vector<double>& v)
{
    std::vector<double> n(v.size());
    double maxVal = 0.0;
    for (u32 i = 0; i < v.size(); i++)
        maxVal = std::max(std::fabs(v[i]), maxVal);
    if (maxVal < 1.0)
        maxVal = 1.0;
    for (u32 i = 0; i < v.size(); i++)
        n[i] = v[i] / maxVal;
    return n;
}


void test1(const tTest& t)
{
    u32 sampleRate = 8000;
    u32 numSamples = (u32) (sampleRate * 3.5);

    audio::tWaveMaker m(numSamples, sampleRate);

    {
        double frequency = 1000.0;
        double amplitute = 2.4;
        m.addWave(frequency, amplitute);
    }

    {
        double frequency = 400.0;
        double amplitute = 1.6;
        m.addWave(frequency, amplitute);
    }

    std::vector<double> left, right, combined;

    left = m.getLeft();
    right = m.getRight();
    combined = m.getWave();

    t.iseq(left, combined);
    t.iseq((int)right.size(), 0);
    t.reject(m.isStereo());

    m.writeToFile(gWavePath);
    gWave = normalize(combined);
}


void test2(const tTest& t)
{
    audio::tWaveMaker m = audio::tWaveMaker::readFromFile(gWavePath);

    std::vector<double> left, right, combined;

    left = m.getLeft();
    right = m.getRight();
    combined = m.getWave();

    t.iseq(left, combined);
    t.iseq((int)right.size(), 0);
    t.reject(m.isStereo());

    t.iseq(combined, gWave, 1e-4);
}


int main()
{
    tCrashReporter::init();

    tTest("WaveMaker test1", test1);
    tTest("WaveMaker test2", test2);

    return 0;
}
