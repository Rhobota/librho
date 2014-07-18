#ifndef __rho_audio_ebAudio_h__
#define __rho_audio_ebAudio_h__


#include <rho/ppcheck.h>
#include <rho/eRho.h>


namespace rho
{
namespace audio
{


class ebAudio : public ebObject
{
    public:

        ebAudio(std::string reason) : ebObject(reason) { }

        ~ebAudio() throw() { }
};


class eWavefileReadError : public ebAudio
{
    public:

        eWavefileReadError() : ebAudio(
                "An error occurred reading the wave file.") { }

        ~eWavefileReadError() throw() { }
};


class eWavefileWriteError : public ebAudio
{
    public:

        eWavefileWriteError() : ebAudio(
                "An error occurred writing the wave file.") { }

        ~eWavefileWriteError() throw() { }
};


class eAudioFileFormatError : public ebAudio
{
    public:

        eAudioFileFormatError(std::string reason) : ebAudio(reason) { }

        ~eAudioFileFormatError() throw() { }
};


}     // namespace audio
}     // namespace rho


#endif   // __rho_audio_ebAudio_h__
