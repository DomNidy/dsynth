// Contains type declarations for audio file headers & structs
#include <stdio.h>
#include <stdint.h>
#include <freqs.h>

// Struct that stores the file header for .wav audio file
// Reference: http://soundfile.sapp.org/doc/WaveFormat/
typedef struct WavHeader
{
    // "RIFF" chunk descriptor
    // The format of concern here is "WAVE" which requires two sub-chunks: "fmt" and "data"
    char chunkId[4];    // contains the letters "RIFF" (big endian)
    uint32_t chunkSize; // size of the rest of the chunk following this number (36 + SubChunk2 size)
    char format[4];     // contains the letters "WAVE" (big endian)

    // "fmt" sub-chunk
    char subchunk1Id[4];    // contains the letters "fmt"
    uint32_t subchunk1Size; // 16 for PCM, this is the size of the rest of the subchunk which follows this number
    uint16_t audioFormat;   // PCM = 1 (i.e. linear quantization) values other than 1 indicate some form of compression
    uint16_t numChannels;   // Mono = 1, stereo = 2, etc.
    uint32_t sampleRate;    // 8000, 44100, etc. the number of audio samples to write/play per second (hz)
    uint32_t byteRate;      // == sample_rate * num_channels * (bits_per_sample/8). the amount of bytes per second of audio
    uint32_t blockAlign;    // == numChannels * (bitsPerSample/8), The number of bytes for one sample including all channels.
    uint16_t bitsPerSample; // 8 bits = 8, 16 bits = 16, etc.

    // "data" sub chunk
    char subchunk2Id[4];    // contains the letters "data"
    uint32_t subchunk2Size; // Number of bytes in the data section (following this)

} WavHeader;

// union allocates memory for the largest member only
// can only store 1 of the values here at a time
// using union to allow polymorphic typing
typedef union AudioFileHeader
{
    WavHeader wavHeader;
} AudioFileHeader;

// Interface which tells us how to create and write to arbitrary audio file types
typedef struct AudioFile
{
    FILE *audioFile;        // audio file we create and write music to
    AudioFileHeader header; // contains the file header used to create the audioFile

    uint32_t samplesWritten; // number of audio samples we've written so far
    double bpm;              // beats per minute of the song in the audio file
    uint32_t samplesPerBeat;
} AudioFile;

// Function declarations

// Creates a .wav file and returns an AudioFile struct pointer
AudioFile *createWavFile(const char *fileName, uint32_t sampleRate, uint16_t bitsPerSample, uint16_t numChannels, double bpm);

// TODO: We create an abstraction for the oscillator & sample generation
// TODO: We should buffer the generated samples, then write them all to the file at once
// Use an oscillator to generate `numSamples` samples of a sine wave, then write them to AudioFile
void writeSineWave(AudioFile *audioFile, freq_t freq1, uint32_t numSamples);

void printAudioFile(const AudioFile *audioFile);
void printWavHeader(const WavHeader *header);