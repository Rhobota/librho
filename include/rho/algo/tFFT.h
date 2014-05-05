#ifndef __rho_algo_tFFT_h__
#define __rho_algo_tFFT_h__


#include <rho/types.h>

#include <utility>
#include <vector>


namespace rho
{
namespace algo
{


/**
 * Fast Fourier Transform
 *
 * Converts a time-domain discrete signal into a frequency-domain discrete
 * signal. The frequency-domain signal is stored as descriptions of
 * sinusoids. Each sinusoid has a real part, an imaginary part, a frequency,
 * an amplitude, and a phase angle.
 */
class tFFT
{
    public:

        /**
         * Converts a time-domain discrete signal into a frequency-domain
         * discrete signal. The frequency-domain signal is stored as
         * descriptions of sinusoids. Each sinusoid has a real part, an
         * imaginary part, a frequency, an amplitude, and a phase angle.
         *
         * @param  input       the discrete time-domain signal
         * @param  sampleRate  the sample rate the signal was sampled at
         */
        tFFT(std::vector<double> input, int sampleRate);

        /**
         * Returns the real parts of the frequency-domain signal.
         */
        std::vector<double> getRealPart();

        /**
         * Returns the imaginary parts of the frequency-domain signal.
         */
        std::vector<double> getImaginaryPart();

        /**
         * Returns the frequencies of the sinusoids.
         */
        std::vector<double> getFrequencies();

        /**
         * Returns the amplitudes of the sinusoids.
         */
        std::vector<double> getAmplitudes();

        /**
         * Returns the frequency and amplitude pairs sorted by frequency.
         */
        std::vector< std::pair<double, double> > getFreqAmpPairs();

        /**
         * Returns the amplitude and frequency pairs sorted by amplitude.
         */
        std::vector< std::pair<double, double> > getAmpFreqPairs();

        /**
         * Returns the phases of the sinusoids.
         */
        std::vector<double> getPhases();

        /**
         * Returns the sample rate that the input signal was sampled at.
         */
        int getSampleRateOfInputSignal();

        /**
         * Converts backwards from a frequency-domain signal to a time-domain
         * signal. The frequency-domain signal is given as a list of real
         * and imaginary parts of several sinusoids.
         *
         * @param  realPart  the real parts of the sinusoids
         * @param  imgPart   the imaginary parts of the sinusoids
         * @return           a discrete time-domain signal
         */
        static
        std::vector<double> inverse(
                std::vector<double> realPart,
                std::vector<double> imgPart);

        /**
         * Calculates the floor of the base-2 logarithm of an integer.
         *
         * @param  n  the integer to calculate on
         * @return    the floor of the base-2 logarithm of 'n'
         */
        static
        int floorlog2(int n);

        /**
         * Determines whether the given integer is a power of two
         *
         * @param  n  the integer to calculate on
         * @return    true if 'n' is a power of two, false otherwise
         */
        static
        bool isPowerOf2(u64 n);

    private:

        std::vector<double> m_cosPart;
        std::vector<double> m_sinPart;

        std::vector<double> m_frequencies;
        std::vector<double> m_amplitudes;
        std::vector<double> m_phases;

        int m_sampleRate;
};


/**
 * Discrete Fourier Transform
 * (is just a proof of concept, it is super slow, use the FFT in real life)
 */
class tDFT
{
    public:

        /**
         * Converts time-domain samples of a signal to frequency-domain
         * sinusoids.
         *
         * @param  input       the time-domain input signal (in discrete
         *                     samples)
         * @param  sampleRate  the rate that the signal was sampled at
         */
        tDFT(std::vector<double> input, int sampleRate);

        /**
         * Returns the real part of the analysis (the cosine part).
         */
        std::vector<double> getRealPart();

        /**
         * Returns the imaginary part of the Fourier analysis (the sine part).
         */
        std::vector<double> getImaginaryPart();

        /**
         * Returns the frequencies of the sinusoids analysed.
         */
        std::vector<double> getFrequencies();

        /**
         * Returns the amplitudes of the sinusoids analysed.
         */
        std::vector<double> getAmplitudes();

        /**
         * Returns the phases of the sinusoids analysed.
         */
        std::vector<double> getPhases();

        /**
         * Returns the sample rate that the input signal was sampled at.
         */
        int getSampleRateOfInputSignal();

        /**
         * Converts from the real and imaginary parts of a frequency-domain
         * signal to time-domain samples of the signal.
         *
         * @param  realPart  the real part of a frequency-domain signal
         * @param  imgPart   the imaginary part of a frequency-domain signal
         * @return           time-domain samples of the signal
         */
        static
        std::vector<double> inverse(
                std::vector<double> realPart,
                std::vector<double> imgPart);

    private:

        std::vector<double> m_cosPart;
        std::vector<double> m_sinPart;

        std::vector<double> m_frequencies;
        std::vector<double> m_amplitudes;
        std::vector<double> m_phases;

        int m_sampleRate;
};


/**
 * Discrete Sine Transform
 * (is only a proof of concept, it is very slow... use the FFT in real life)
 */
class tDST
{
    public:

        /**
         * Converts from the time-domain to the frequency-domain.
         * That is, this constructor takes sample of a signal over time and
         * converts it to descriptions of sine waves (frequencies and
         * amplitudes).
         *
         * @param  input       amplitude samples in the time-domain
         * @param  sampleRate  the sample rate of 'input'
         */
        tDST(std::vector<double> input, int sampleRate);

        /**
         * Returns the frequencies of the input signal.
         */
        std::vector<double> getFrequencies();

        /**
         * Returns the amplitudes of the input signal.
         */
        std::vector<double> getAmplitudes();

        /**
         * Returns the sample rate of the original input signal.
         */
        int getSampleRateOfOriginalSignal();

        /**
         * Converts backwards (from the frequency-domain to the time-domain).
         * That is, it takes an array of amplitudes of sine wave, and converts
         * them back to time-domain samples of a signal.
         *
         * @param  amplitudes  the amplitudes of the sine waves
         * @return             samples of the signal
         */
        static
        std::vector<double> inverse(std::vector<double> amplitudes);

    private:

        std::vector<double> m_frequencies;
        std::vector<double> m_amplitudes;
        int m_sampleRate;
};


}   // namespace algo
}   // namespace rho


#endif    // __rho_algo_tFFT_h__
