#include <rho/algo/tFFT.h>
#include <rho/algo/ebAlgo.h>

#include "fft_algorithm.ipp"

#include <algorithm>


using namespace std;


namespace rho
{
namespace algo
{


tFFT::tFFT(vector<double> input, int sampleRate)
    : m_sampleRate(sampleRate)
{
    if (! isPowerOf2(input.size()))
    {
        throw eInvalidArgument("You must input an array with a power-of-two "
                               "size to the FFT algorithm.");
    }

    // Map the input vector to what the FFT algorithm expects (an array).
    int fftFrameSize = input.size();
    double* fftBuffer = new double[2*fftFrameSize];
    for (int i = 0; i < fftFrameSize; i++)
    {
        fftBuffer[2*i] = input[i];
        fftBuffer[2*i+1] = 0.0;
    }

    // Run the FFT algorithm.
    fft_algorithm(fftBuffer, fftFrameSize, -1);

    // Map the output of the FFT algorithm back to vectors.
    for (int i = 0; i < fftFrameSize; i++)
    {
        m_cosPart.push_back(fftBuffer[2*i]);
        m_sinPart.push_back(fftBuffer[2*i + 1]);
    }

    // Clean up stuff.
    delete [] fftBuffer;

    // Calculate useful things.
    int len = m_cosPart.size() / 2;
    for (int i = 0; i < len; i++)
    {
        m_frequencies.push_back((double)i * sampleRate / fftFrameSize);
        m_amplitudes.push_back(hypot(m_cosPart[i], m_sinPart[i]));
        m_amplitudes[i] /= fftFrameSize/2.0;
        m_phases.push_back(180.0*atan2(m_sinPart[i], m_cosPart[i]) / M_PI - 90);
    }
}


vector<double> tFFT::getRealPart()
{
    return m_cosPart;
}


vector<double> tFFT::getImaginaryPart()
{
    return m_sinPart;
}


vector<double> tFFT::getFrequencies()
{
    return m_frequencies;
}


vector<double> tFFT::getAmplitudes()
{
    return m_amplitudes;
}


vector< pair<double, double> > tFFT::getFreqAmpPairs()
{
    vector< pair<double, double> > waves(m_frequencies.size());
    for (size_t i = 0; i < m_frequencies.size(); i++)
        waves[i] = make_pair(m_frequencies[i], m_amplitudes[i]);
    return waves;
}


vector< pair<double, double> > tFFT::getAmpFreqPairs()
{
    vector< pair<double, double> > afp = getFreqAmpPairs();
    for (size_t i = 0; i < afp.size(); i++)
        swap(afp[i].first, afp[i].second);
    sort(afp.begin(), afp.end());
    return afp;
}


vector<double> tFFT::getPhases()
{
    return m_phases;
}


vector<double> tFFT::inverse(vector<double> realPart, vector<double> imgPart)
{
    if (realPart.size() != imgPart.size())
    {
        throw eInvalidArgument("The real and imaginary array must be the "
                               "same size.");
    }

    // Map the input vectors to what the FFT algorithm wants
    // (which is an array).
    int fftFrameSize = realPart.size();
    double* fftBuffer = new double[2*fftFrameSize];
    for (int i = 0; i < fftFrameSize; i++)
    {
        fftBuffer[2*i] = realPart[i];
        fftBuffer[2*i+1] = imgPart[i];
    }

    // Run the FFT algorithm.
    fft_algorithm(fftBuffer, fftFrameSize, 1);

    // Convert the FFT algorithms output back to a signal vector.
    vector<double> signal;
    for (int i = 0; i < fftFrameSize; i++)
    {
        signal.push_back(fftBuffer[2*i] + fftBuffer[2*i+1]);
        signal[i] /= fftFrameSize;
    }

    // Clean up and return.
    delete [] fftBuffer;
    return signal;
}


int tFFT::floorlog2(int n)
{
    int l;
    for (l = -1; n > 0; l++)
        n >>= 1;
    return l;
}


bool tFFT::isPowerOf2(int n)
{
    return (n > 0) && ((n & (n-1)) == 0);
}


tDFT::tDFT(vector<double> input, int sampleRate) : m_sampleRate(sampleRate)
{
    int bin, k;
    double arg;
    int sign = -1;

    int length = input.size();

    for (bin = 0; bin < length; bin++)
    {
        m_cosPart.push_back(0.0);
        m_sinPart.push_back(0.0);

        for (k = 0; k < length; k++)
        {
            arg = 2.0 * bin * M_PI * k / length;
            m_cosPart[bin] += input[k] * cos(arg);
            m_sinPart[bin] += input[k] * sign * sin(arg);
        }

        if (bin < length / 2)
        {
            m_frequencies.push_back((double)bin * sampleRate / length);
            m_amplitudes.push_back(hypot(m_cosPart[bin], m_sinPart[bin]));
            m_amplitudes[bin] /= length/2.0;
            m_phases.push_back(
                    180.0*atan2(m_sinPart[bin], m_cosPart[bin])/M_PI-90);
        }
    }
}


vector<double> tDFT::getRealPart()
{
    return m_cosPart;
}


vector<double> tDFT::getImaginaryPart()
{
    return m_sinPart;
}


vector<double> tDFT::getFrequencies()
{
    return m_frequencies;
}


vector<double> tDFT::getAmplitudes()
{
    return m_amplitudes;
}


vector<double> tDFT::getPhases()
{
    return m_phases;
}


int tDFT::getSampleRateOfInputSignal()
{
    return m_sampleRate;
}


vector<double> tDFT::inverse(vector<double> realPart, vector<double> imgPart)
{
    if (realPart.size() != imgPart.size())
    {
        throw eInvalidArgument("The real and imaginary array must be the "
                               "same size.");
    }

    int length = realPart.size();

    int bin, k;
    double arg;
    int sign = -1;

    vector<double> cosPart, sinPart;

    vector<double> signal;

    for (bin = 0; bin < length; bin++)
    {
        cosPart.push_back(0.0);
        sinPart.push_back(0.0);

        for (k = 0; k < length; k++)
        {
            arg = 2.0 * bin * M_PI * k / length;
            cosPart[bin] += realPart[k] * cos(arg);
            sinPart[bin] += imgPart[k] * sign * sin(arg);
        }

        signal.push_back(cosPart[bin] + sinPart[bin]);
        signal[bin] /= length;
    }

    return signal;
}


tDST::tDST(vector<double> input, int sampleRate) : m_sampleRate(sampleRate)
{
    int length = input.size();

    m_frequencies.push_back(0.0);
    m_amplitudes.push_back(input[0]);

    for (int bin = 1; bin < length; bin++)
    {
        // Get the frequency with respect to real time (seconds).
        m_frequencies.push_back((double)bin * sampleRate / (2*length));

        // Zero the value we are about to calculate.
        m_amplitudes.push_back(0.0);

        // Calculate this partial!
        for (int k = 0; k < length; k++)
        {
            double arg = bin * M_PI * k / length;
            m_amplitudes[bin] += input[k] * sin(arg);
        }

        m_amplitudes[bin] /= length / 2.0;
    }
}


vector<double> tDST::getFrequencies()
{
    return m_frequencies;
}


vector<double> tDST::getAmplitudes()
{
    return m_amplitudes;
}


int tDST::getSampleRateOfOriginalSignal()
{
    return m_sampleRate;
}


vector<double> tDST::inverse(vector<double> amplitudes)
{
    vector<double> samples;
    samples.push_back(amplitudes[0]);

    int length = amplitudes.size();

    for (int bin = 1; bin < length; bin++)
    {
        samples.push_back(0.0);
        for (int k = 0; k < length; k++)
        {
            double arg = bin * M_PI * k / length;
            samples[bin] += amplitudes[k] * sin(arg);
        }
    }

    return samples;
}


}    // namespace algo
}    // namespace rho
