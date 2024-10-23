#include "audiofile.h"
#include <stdlib.h>

const double PI = 3.14159265358979323846;

// do not call this, method used by `createWavFile` to create a file and write wavheader to it
FILE *_createFileAndWriteWavHeader(const WavHeader *header, const char *fileName)
{
    FILE *file = fopen(fileName, "wb");
    if (!file)
    {
        perror("Failed to create .wav file");
        return 1;
    }

    // Write the RIFF header
    fwrite(&header->chunkId, 1, 4, file);
    fwrite(&header->chunkSize, 4, 1, file);
    fwrite(&header->format, 1, 4, file);

    // Write the "fmt " subchunk
    fwrite(&header->subchunk1Id, 1, 4, file);
    fwrite(&header->subchunk1Size, 4, 1, file);
    fwrite(&header->audioFormat, 2, 1, file);
    fwrite(&header->numChannels, 2, 1, file);
    fwrite(&header->sampleRate, 4, 1, file);
    fwrite(&header->byteRate, 4, 1, file);
    fwrite(&header->blockAlign, 2, 1, file);
    fwrite(&header->bitsPerSample, 2, 1, file);

    // Write the "data" subchunk
    fwrite(&header->subchunk2Id, 1, 4, file);
    fwrite(&header->subchunk2Size, 4, 1, file);

    return file;
}

AudioFile *createWavFile(const char *fileName, uint32_t sampleRate, uint16_t bitsPerSample, uint16_t numChannels, double bpm)
{
    // allocate memory for AudioFile struct
    AudioFile *audioFile = (AudioFile *)malloc(sizeof(AudioFile));

    if (audioFile == NULL)
    {
        perror("Failed to allocate memory for AudioFile");
    }

    audioFile->bpm = bpm;
    audioFile->samplesWritten = 0;
    audioFile->samplesPerBeat = sampleRate * 60 / bpm;

    // "RIFF" chunk descriptor
    *audioFile->header.wavHeader.chunkId = "RIFF";
    audioFile->header.wavHeader.chunkSize = 36; // initially set to 36 as no data is present in file yet
    *audioFile->header.wavHeader.format = "WAVE";

    // "fmt" sub-chunk
    *audioFile->header.wavHeader.subchunk1Id = "fmt ";
    audioFile->header.wavHeader.subchunk1Size = 16;
    audioFile->header.wavHeader.audioFormat = 1;           // 1 for PCM
    audioFile->header.wavHeader.numChannels = numChannels; // 1 for mono, 2 for stereo, etc.
    audioFile->header.wavHeader.sampleRate = sampleRate;
    audioFile->header.wavHeader.byteRate = sampleRate * numChannels * (bitsPerSample / 8);
    audioFile->header.wavHeader.blockAlign = numChannels * (bitsPerSample / 8);
    audioFile->header.wavHeader.bitsPerSample = bitsPerSample;

    // "data" sub-chunk
    *audioFile->header.wavHeader.subchunk2Id = "data";
    audioFile->header.wavHeader.subchunk2Size = 0; // initially 0 as no samples have been written yet

    // create the file
    audioFile->audioFile = _createFileAndWriteWavHeader(&audioFile->header.wavHeader, fileName);

    return audioFile;
}

void writeSineWave(AudioFile *audioFile, freq_t freq1, uint32_t numSamples)
{
    // maximum amplitude is defined by bits per sample
    int amplitude = (2 << audioFile->header.wavHeader.bitsPerSample) - 1;
    // the amount of bytes written to the file
    int bytesWritten = 0;

    // Generate the sine wave samples and write them to the file
    for (int i = 0; i < numSamples; ++i)
    {
        // Time variable
        double t = (double)i / audioFile->header.wavHeader.sampleRate;

        // Generate the sample (sine wave formula)
        double sample = amplitude * sin(2.0 * PI * freq1 * t);

        // TODO: File writing logic should allow for multiple channels
        // Cast the sample sample_t
        switch (audioFile->header.wavHeader.bitsPerSample)
        {
        case 8:
            fwrite((int8_t*)&sample, sizeof(int8_t), 1, audioFile->audioFile);
            bytesWritten += 1;
            break;
        case 16:
            fwrite((int16_t*)&sample, sizeof(int16_t), 1, audioFile->audioFile);
            bytesWritten += 2;
            break;
        case 24:
            // Handle 24-bit PCM as special case, it's usually packed into 3 bytes
            int32_t sampleCasted = (int32_t)(sample * (1 << 8));
            fwrite((uint8_t *)&sampleCasted + 1, 3, 1, audioFile->audioFile);
            bytesWritten += 3;
            break;
        case 32:
            fwrite((int32_t*)&sample, sizeof(int32_t), 1, audioFile->audioFile);
            bytesWritten += 4;
            break;
        default:
            fprintf(stderr, "Unsupported bits per sample: %d\n", audioFile->header.wavHeader.bitsPerSample);
        }

        audioFile->samplesWritten++;
    }

    printf("after writing\n");
    // update the subchunk2 (data subchunk) size in the header struct
    audioFile->header.wavHeader.subchunk2Size += bytesWritten;
    // set the offset in file header to 40 bytes (position of subchunk2size)
    fseek(audioFile->audioFile, 40, SEEK_SET);

    // increment the subchunk size stored in the audioFile header to reflect the subchunk2size stored in the header struct
    fwrite(&audioFile->header.wavHeader.subchunk2Size, 4, 1, audioFile->audioFile);

    // reset the seek position in file
    fseek(audioFile->audioFile, 0, SEEK_END);
}

void printWavHeader(const WavHeader *header)
{
    printf("Chunk ID: %.4s\n", header->chunkId);
    printf("Chunk Size: %u\n", header->chunkSize);
    printf("Format: %.4s\n", header->format);
    printf("Subchunk1 ID: %.4s\n", header->subchunk1Id);
    printf("Subchunk1 Size: %u\n", header->subchunk1Size);
    printf("Audio Format: %u\n", header->audioFormat);
    printf("Number of Channels: %u\n", header->numChannels);
    printf("Sample Rate: %u\n", header->sampleRate);
    printf("Byte Rate: %u\n", header->byteRate);
    printf("Block Align: %u\n", header->blockAlign);
    printf("Bits Per Sample: %u\n", header->bitsPerSample);
    printf("Subchunk2 ID: %.4s\n", header->subchunk2Id);
    printf("Subchunk2 Size: %u\n", header->subchunk2Size);
}

void printAudioFile(const AudioFile *audioFile)
{
    if (audioFile == NULL)
    {
        printf("AudioFile is NULL.\n");
        return;
    }
    printf("Audio File: %p\n", audioFile->audioFile);
    printf("Samples Written: %u\n", audioFile->samplesWritten);
    printf("BPM: %.2f\n", audioFile->bpm);
    printf("Samples Per Beat: %u\n", audioFile->samplesPerBeat);
    printf("Wav Header:\n");
    printWavHeader(&audioFile->header.wavHeader);
}