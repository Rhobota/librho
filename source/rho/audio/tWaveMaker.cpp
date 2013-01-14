#include <rho/audio/tWaveMaker.h>
#include <rho/audio/ebAudio.h>

#include <cmath>
#include <fstream>

using namespace std;


namespace rho
{
namespace audio
{


// Kind of dumb function, but it matches the other write___() functions.
static
void writeStr(ostream& out, string str)
{
    out << str;
    if (!out)
        throw eWavefileWriteError();
}

// Writes in little-endian
static
void writeInt(ostream& out, u32 i)
{
    out.put((u8) (i & 0xFF));
    out.put((u8) ((i & 0xFF00) >> 8));
    out.put((u8) ((i & 0xFF0000)) >> 16);
    out.put((u8) ((i & 0xFF000000) >> 24));
    if (!out)
        throw eWavefileWriteError();
}

// Writes in little-endian
static
void writeShort(ostream& out, u16 i)
{
    out.put((u8) (i & 0xFF));
    out.put((u8) ((i & 0xFF00) >> 8));
    if (!out)
        throw eWavefileWriteError();
}

// Another dumb function, but it matches the other write___() functions.
static
void writeByte(ostream& out, u8 i)
{
    out.put(i);
    if (!out)
        throw eWavefileWriteError();
}

// Reads a string of the given length
static
string readStr(istream& in, u32 length)
{
    string s;
    char c;
    for (u32 i = 0; i < length; i++)
    {
        in.get(c);
        s += c;
    }
    if (!in)
        throw eWavefileReadError();
    return s;
}

// Reads in little-endian
static
u32 readInt(istream& in)
{
    char a, b, c, d;
    in.get(a); in.get(b); in.get(c); in.get(d);
    u32 i = 0;
    i |= (0xFF & d);
    i <<= 8;
    i |= (0xFF & c);
    i <<= 8;
    i |= (0xFF & b);
    i <<= 8;
    i |= (0xFF & a);
    if (!in)
        throw eWavefileReadError();
    return i;
}

// Reads in little-endian
static
u16 readShort(istream& in)
{
    char a, b;
    in.get(a); in.get(b);
    u16 i = 0;
    i |= (0xFF & b);
    i <<= 8;
    i |= (0xFF & a);
    if (!in)
        throw eWavefileReadError();
    return i;
}

static
u8 readByte(istream& in)
{
    char c;
    in.get(c);
    if (!in)
        throw eWavefileReadError();
    return c;
}

// Takes values in [-1, 1] only.
static
void makeWAV(ostream& out, u32 bitsPerSample, u32 sampleRate,
             const vector<double>& sound)
{
    if (bitsPerSample != 8 && bitsPerSample != 16)
        throw eInvalidArgument("You may only write 8 or 16 bits/sample.");

    u32 subChunk1Size = 16;
    u32 audioFormat = 1;
    u32 numChannels = 1;
    u32 byteRate = sampleRate * numChannels * bitsPerSample / 8;
    u32 blockAlign = numChannels * bitsPerSample / 8;
    u32 subChunk2Size = sound.size() * numChannels * bitsPerSample / 8;
    u32 chunkSize = 4 + (8 + subChunk1Size) + (8 + subChunk2Size);

    // See https://ccrma.stanford.edu/courses/422/projects/WaveFormat/

    writeStr(out, "RIFF");
    writeInt(out, chunkSize);
    writeStr(out, "WAVE");
    writeStr(out, "fmt ");
    writeInt(out, subChunk1Size);
    writeShort(out, audioFormat);
    writeShort(out, numChannels);
    writeInt(out, sampleRate);
    writeInt(out, byteRate);
    writeShort(out, blockAlign);
    writeShort(out, bitsPerSample);
    writeStr(out, "data");
    writeInt(out, subChunk2Size);
    if (bitsPerSample == 8)
        for (size_t i = 0; i < sound.size(); i++)
            writeByte( out, (u8) round(255.0 * (sound[i]+1)/2.0) );
    else if (bitsPerSample == 16)
        for (size_t i = 0; i < sound.size(); i++)
            writeShort( out, (u16) round(sound[i] * ((1 << 15) - 1)) );
    else
        throw eImpossiblePath();
}

// Takes values in [-1, 1] only.
static
void makeWAV(ostream& out, u32 bitsPerSample, u32 sampleRate,
             const vector<double>& left, const vector<double>& right)
{
    if (bitsPerSample != 8 && bitsPerSample != 16)
        throw eInvalidArgument("You may only write 8 or 16 bits/sample.");

    if (left.size() != right.size())
        throw eInvalidArgument("left and right must be of equal lenghts");

    u32 subChunk1Size = 16;
    u32 audioFormat = 1;
    u32 numChannels = 2;
    u32 byteRate = sampleRate * numChannels * bitsPerSample / 8;
    u32 blockAlign = numChannels * bitsPerSample / 8;
    u32 subChunk2Size = left.size() * numChannels * bitsPerSample / 8;
    u32 chunkSize = 4 + (8 + subChunk1Size) + (8 + subChunk2Size);

    // See https://ccrma.stanford.edu/courses/422/projects/WaveFormat/

    writeStr(out, "RIFF");
    writeInt(out, chunkSize);
    writeStr(out, "WAVE");
    writeStr(out, "fmt ");
    writeInt(out, subChunk1Size);
    writeShort(out, audioFormat);
    writeShort(out, numChannels);
    writeInt(out, sampleRate);
    writeInt(out, byteRate);
    writeShort(out, blockAlign);
    writeShort(out, bitsPerSample);
    writeStr(out, "data");
    writeInt(out, subChunk2Size);
    if (bitsPerSample == 8)
        for (size_t i = 0; i < left.size(); i++)
            writeByte( out, (u8) round(255.0*(left[i]+1)/2.0) ),
                writeByte( out, (u8) round(255.0*(right[i]+1)/2.0) );
    else if (bitsPerSample == 16)
        for (size_t i = 0; i < left.size(); i++)
            writeShort( out, (u16) round(left[i] * ((1 << 15) - 1)) ),
                writeShort( out, (u16) round(right[i] * ((1 << 15) - 1)) );
    else
        throw eImpossiblePath();
}

static
void readWAV(istream& in, u32& sampleRate, u32& numChannels,
             vector<double>& left, vector<double>& right)
{
    if (readStr(in, 4) != "RIFF")     // chunk id
        throw eAudioFileFormatError("Expected 'RIFF'");

    readInt(in);                      // chunk size

    if (readStr(in, 4) != "WAVE")     // format
        throw eAudioFileFormatError("Expected 'WAVE'");

    if (readStr(in, 4) != "fmt ")     // subchunk1 id
        throw eAudioFileFormatError("Expected 'fmt '");

    if (readInt(in) != 16)            // subchunk1 size
        throw eAudioFileFormatError("Expected subchunk1 to be 16");

    if (readShort(in) != 1)           // audio format (PCM == 1)
        throw eAudioFileFormatError("Expected PCM to be 1");

    numChannels = readShort(in);
    if (numChannels != 1 && numChannels != 2)
        throw eAudioFileFormatError("Expected num channels to be 1 or 2");

    sampleRate = readInt(in);

    readInt(in);     // byte rate (derivable from the other stuff we have)

    readShort(in);   // blockAlign (also derivable from other stuff we have)

    u32 bitsPerSample = readShort(in);
    if (bitsPerSample != 8 && bitsPerSample != 16)
        throw eAudioFileFormatError("Expected bits/sample to be 8 or 16");

    if (readStr(in, 4) != "data")      // subchunk2 id
        throw eAudioFileFormatError("Expected 'data'");

    u32 subChunk2Size = readInt(in);   // numSamples*numChannels*bitsPerSamp/8;
    u32 length = subChunk2Size / numChannels / bitsPerSample * 8;

    left = vector<double>(length);
    right = vector<double>(length);

    if (bitsPerSample == 8)
    {
        if (numChannels == 1)
        {
            for (u32 i = 0; i < length; i++)
            {
                u8 c = readByte(in);
                left[i] = (c / 255.0 - 0.5) * 2;
                right[i] = left[i];
            }
        }
        else
        {
            for (u32 i = 0; i < length; i++)
            {
                u8 c = readByte(in);
                left[i] = (c / 255.0 - 0.5) * 2;
                c = readByte(in);
                right[i] = (c / 255.0 - 0.5) * 2;
            }
        }
    }
    else if (bitsPerSample == 16)
    {
        if (numChannels == 1)
        {
            for (u32 i = 0; i < length; i++)
            {
                i16 c = readShort(in);
                left[i] = c / 32767.0;
                right[i] = left[i];
            }
        }
        else
        {
            for (u32 i = 0; i < length; i++)
            {
                i16 c = readShort(in);
                left[i] = c / 32767.0;
                c = readShort(in);
                right[i] = c / 32767.0;
            }
        }
    }
    else
    {
        throw eImpossiblePath();
    }
}


tWaveMaker::tWaveMaker(u32 numSamples, u32 sampleRate)
{
    m_numSamples = numSamples;
    m_sampleRate = sampleRate;
    m_left = vector<double>(numSamples, 0.0);
    m_stereo = false;
}

tWaveMaker::tWaveMaker(vector<double> samples, u32 sampleRate)
{
    m_sampleRate = sampleRate;
    m_left = samples;
    m_numSamples = samples.size();
    m_stereo = false;
}

tWaveMaker::tWaveMaker(vector<double> left, vector<double> right,
                       u32 sampleRate)
{
    if (left.size() != right.size())
    {
        throw eInvalidArgument("left and right must be of equal lenghts");
    }

    m_sampleRate = sampleRate;
    m_left = left;
    m_right = right;
    m_numSamples = left.size();
    m_stereo = true;
}

void tWaveMaker::addWave(double frequency, double amplitude)
{
    vector<double> newWave =
        genNumSamples(frequency, m_sampleRate, amplitude, m_numSamples);
    if (m_stereo)
        for (size_t i = 0; i < newWave.size(); i++)
            m_left[i] += newWave[i],
                m_right[i] += newWave[i];
    else
        for (size_t i = 0; i < newWave.size(); i++)
            m_left[i] += newWave[i];
}

vector<double> tWaveMaker::getWave()
{
    if (m_stereo)
    {
        vector<double> combined(m_numSamples, 0);
        for (u32 i = 0; i < m_numSamples; i++)
            combined[i] = (m_left[i] + m_right[i]) / 2.0;
        return combined;
    }
    else
    {
        return m_left;
    }
}

vector<double> tWaveMaker::getLeft()
{
    return m_left;
}

vector<double> tWaveMaker::getRight()
{
    return m_right;
}

u32 tWaveMaker::getSampleRate()
{
    return m_sampleRate;
}

u32 tWaveMaker::getNumSamples()
{
    return m_numSamples;
}

bool tWaveMaker::isStereo()
{
    return m_stereo;
}

void tWaveMaker::writeToFile(string path)
{
    // Scale the wave down to [-1, 1] if needed.
    double maxAmplitude = 0.0;
    for (size_t i = 0; i < m_left.size(); i++)
    {
        maxAmplitude = max(maxAmplitude, fabs(m_left[i]));
        if (m_stereo)
            maxAmplitude = max(maxAmplitude, fabs(m_right[i]));
    }
    if (maxAmplitude < 1.0)
        maxAmplitude = 1.0;

    vector<double> scaledLeft = m_left;
    vector<double> scaledRight = m_right;
    for (size_t i = 0; i < scaledLeft.size(); i++)
        scaledLeft[i] /= maxAmplitude;
    for (size_t i = 0; i < scaledRight.size(); i++)
        scaledRight[i] /= maxAmplitude;

    // Write the file.
    ofstream out(path.c_str(), ios::out | ios::binary);
    if (m_stereo)
        makeWAV(out, 16, m_sampleRate, scaledLeft, scaledRight);
    else
        makeWAV(out, 16, m_sampleRate, scaledLeft);
    out.close();
}

tWaveMaker tWaveMaker::readFromFile(string path)
{
    ifstream in(path.c_str(), ios::in | ios::binary);
    vector<double> left, right;
    u32 sampleRate;
    u32 numChannels;
    readWAV(in, sampleRate, numChannels, left, right);
    in.close();

    if (numChannels == 1)
        return tWaveMaker(left, sampleRate);
    else if (numChannels == 2)
        return tWaveMaker(left, right, sampleRate);
    else
        throw eImpossiblePath();
}

vector<double> tWaveMaker::genNumSamples(double frequency, double sampleRate,
                                        double amplitude, u32 numSamples)
{
    vector<double> wave(numSamples);
    for (u32 i = 0; i < numSamples; i++)
        wave[i] = amplitude * sin(2*M_PI*i*frequency/sampleRate);
    return wave;
}

vector<double> tWaveMaker::genNumPhases(double frequency, double sampleRate,
                                       double amplitude, u32 numPhases)
{
    u32 numSamples = (u32) (sampleRate * numPhases / frequency);
    return genNumSamples(frequency, sampleRate, amplitude, numSamples);
}


}   // namespace audio
}   // namespace rho
