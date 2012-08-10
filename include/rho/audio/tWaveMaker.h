#ifndef __rho_audio_tWaveMaker_h__
#define __rho_audio_tWaveMaker_h__


#include <rho/types.h>

#include <string>
#include <vector>


namespace rho
{
namespace audio
{


/**
 * Makes discrete waves and can write the waves to a WAV sound file.
 *
 * Note: If you want to use this to make audible sound, remember that
 *       (1) humans generally can here frequencies from 20Hz to 20kHz, and
 *       (2) you need a good sample rate, probably 8kHz to 44.1kHz.
 */
class tWaveMaker
{
    public:

        /**
         * Creates a new wave that will be recorded at the given sample rate
         * and will contain the given number of samples.
         *
         * The wave will be created with a single channel.
         *
         * @param  numSamples  the number of sample to record
         * @param  sampleRate  the sample rate to create the wave at
         */
        tWaveMaker(u32 numSamples, u32 sampleRate);

        /**
         * Creates a single channel wave initialized with the given samples.
         *
         * @param  samples     the initial samples
         * @param  sampleRate  the sample rate...
         */
        tWaveMaker(std::vector<double> samples, u32 sampleRate);

        /**
         * Creates a stereo wave wave initialized with the given samples.
         *
         * @param  left        the initial left samples
         * @param  right       the initial right samples
         * @param  sampleRate  the sample rate...
         */
        tWaveMaker(std::vector<double> left,
                   std::vector<double> right,
                   u32 sampleRate);

        /**
         * Adds a wave of the given frequency and amplitude.
         * The new wave will be added to the wave that is being constructed
         * already.
         *
         * @param  frequency  the frequency of the new wave that is added
         * @param  amplitude  the amplitude of the new wave that is added
         */
        void addWave(double frequency, double amplitude);

        /**
         * Gets the wave that has been constructed.
         *
         * If the wave is stereo, this method will average the two channels.
         *
         * @return  the constructed wave
         */
        std::vector<double> getWave();

        /**
         * @return  the left channel.
         */
        std::vector<double> getLeft();

        /**
         * @return  the right channel.
         */
        std::vector<double> getRight();

        /**
         * Gets the sample rate of the wave.
         *
         * @return  the sample rate!
         */
        u32 getSampleRate();

        /**
         * Gets the number of samples in the wave.
         *
         * @return  the number of samples in the wave
         */
        u32 getNumSamples();

        /**
         * @return  true if there are two signals (left and right),
         *          false if there is only one signal (left)
         */
        bool isStereo();

        /**
         * Writes the wave to a 'wav' file.
         *
         * @param  path  the file to write to
         * @param  logToConsole  pass true if you want it! (duh)
         */
        void writeToFile(std::string path);

        /**
         * Reads in a wave from a 'wav' file.
         *
         * @param  path  the file to read from
         */
        static
        tWaveMaker readFromFile(std::string path);

        /**
         * Makes a sine wave with a given number of samples.
         *
         * @param  frequency   the frequency of the generated wave (Hz)
         * @param  sampleRate  the rate that the sine wave is sampled (Hz)
         * @param  amplitude   the amplitude of the generated wave
         * @param  numSamples  the number of samples to generate
         */
        static
        std::vector<double> genNumSamples(
                double frequency, double sampleRate,
                double amplitude, u32 numSamples);

        /**
         * Makes a sine wave with with the given number of phases.
         *
         * @param  frequency   the frequency of the generated wave (Hz)
         * @param  sampleRate  the rate that the sine wave is sampled (Hz)
         * @param  amplitude   the amplitude of the generated wave
         * @param  numPhases   the number of phases of the wave to generate
         */
        static
        std::vector<double> genNumPhases(
                double frequency, double sampleRate,
                double amplitude, u32 numPhases);

    private:

        std::vector<double> m_left, m_right;
        u32 m_sampleRate;
        u32 m_numSamples;
        bool m_stereo;
};


}   // namespace audio
}   // namespace rho


#endif    // __rho_audio_tWaveMaker_h__
