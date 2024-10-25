#include "audiofile.h"
#include <stdlib.h>
#include <string.h>

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
    memcpy(audioFile->header.wavHeader.chunkId, "RIFF", 4); // the length of this is 5 but it should be 4, this is because the "" adds a null terminator char after
    audioFile->header.wavHeader.chunkSize = 36;             // initially set to 36 as no data is present in file yet
    memcpy(audioFile->header.wavHeader.format, "WAVE", 4);

    // "fmt" sub-chunk
    memcpy(audioFile->header.wavHeader.subchunk1Id, "fmt ", 4);
    audioFile->header.wavHeader.subchunk1Size = 16;
    audioFile->header.wavHeader.audioFormat = 1;           // 1 for PCM
    audioFile->header.wavHeader.numChannels = numChannels; // 1 for mono, 2 for stereo, etc.
    audioFile->header.wavHeader.sampleRate = sampleRate;
    audioFile->header.wavHeader.byteRate = sampleRate * numChannels * (bitsPerSample / 8);
    audioFile->header.wavHeader.blockAlign = numChannels * (bitsPerSample / 8);
    audioFile->header.wavHeader.bitsPerSample = bitsPerSample;

    // "data" sub-chunk
    memcpy(audioFile->header.wavHeader.subchunk2Id, "data", 4);
    audioFile->header.wavHeader.subchunk2Size = 0; // initially 0 as no samples have been written yet

    // create the file
    audioFile->audioFile = _createFileAndWriteWavHeader(&audioFile->header.wavHeader, fileName);
    if (audioFile->audioFile == NULL)
    {
        perror("Failed to create WAV file");
        free(audioFile);
        return NULL;
    }
    return audioFile;
}

// TODO: This is too tightly coupled to a specific audio file format
// TODO: Should figure out a new pattern to write to files in format agnostic way.
// TODO: Maybe instead, we can simply create an array of custom `Sample` structs,
// TODO: then each format (wav, mp3) can implement their own way of writing them.
void writeSineWave(AudioFile *audioFile, freq_t freq1, uint32_t numSamples)
{
    // maximum amplitude is defined by bits per sample
    int amplitude = (1 << (audioFile->header.wavHeader.bitsPerSample - 1)) - 1;
    printf("writeSineWave:\n"
           "\tmax amplitude: %d\n\n",
           amplitude);
    // the amount of bytes written to the file
    int bytesWritten = 0;

    // Generate the sine wave samples and write them to the file
    for (int i = 0; i < numSamples; ++i)
    {
        // Time variable
        double t = (double)i / audioFile->header.wavHeader.sampleRate;

        // Generate the sample (sine wave formula)
        double sample = amplitude * sin(2.0 * PI * freq1 * t);
        // prevent rounding errors from sample going out of range
        // this just returns: max(-amplitude, min(amplitude, sample))
        sample = (-amplitude > (amplitude < sample ? amplitude : sample)) ? -amplitude : sample;

        // TODO: File writing logic should allow for multiple channels
        // Cast the sample sample_t
        switch (audioFile->header.wavHeader.bitsPerSample)
        {
        case 8:
        {
            int8_t sampleValue = (int8_t)sample;
            fwrite(&sampleValue, sizeof(int8_t), 1, audioFile->audioFile);
            bytesWritten += 1;
            break;
        }
        case 16:
        {
            int16_t sampleValue = (int16_t)sample;
            fwrite(&sampleValue, sizeof(int16_t), 1, audioFile->audioFile);
            bytesWritten += 2;
            break;
        }
        case 24:
        {
            int32_t sampleValue = (int32_t)(sample * (1 << 8)); // scale up for 24 bits
            fwrite((uint8_t *)&sampleValue + 1, 3, 1, audioFile->audioFile);
            bytesWritten += 3;
            break;
        }
        case 32:
        {
            int32_t sampleValue = (int32_t)sample;
            fwrite(&sampleValue, sizeof(int32_t), 1, audioFile->audioFile);
            bytesWritten += 4;
            break;
        }
        default:
            fprintf(stderr, "Unsupported bits per sample: %d\n", audioFile->header.wavHeader.bitsPerSample);
        }

        audioFile->samplesWritten++;
    }

    setDataChunkSize(&audioFile->header.wavHeader, audioFile->audioFile, audioFile->header.wavHeader.subchunk2Size + bytesWritten);
}

void setDataChunkSize(WavHeader *fileHeader, FILE *file, uint32_t newSize)
{
    if (!file)
    {
        fprintf(stderr, "setDataChunkSize received NULL file");
        return;
    }

    fileHeader->subchunk2Size = newSize;

    // seek to the position of data chunk size (40 bytes offset from beginning of file)
    if (fseek(file, 40, SEEK_SET) != 0)
    {
        fprintf(stderr, "Error seeking to position of subchunk 2 size\n");
        return;
    }

    // set subchunk 2 size
    if (fwrite(&fileHeader->subchunk2Size, 4, 1, file) != 1)
    {
        fprintf(stderr, "Error writing subchunk 2 size\n");
        return;
    }

    // update the main chunk size
    fileHeader->chunkSize = 36 + fileHeader->subchunk2Size;

    // set offset to position of chunk size (4 bytes from start of file)
    if (fseek(file, 4, SEEK_SET) != 0)
    {
        fprintf(stderr, "Error seeking to position of chunk size\n");
        return;
    }

    // set chunksize
    if (fwrite(&fileHeader->chunkSize, 4, 1, file) != 1)
    {
        fprintf(stderr, "Error writing chunk size\n");
        return;
    }

    // reset seek position to end of the file
    if (fseek(file, 0, SEEK_END) != 0)
    {
        fprintf(stderr, "Error seeking to end of file\n");
        return;
    }
}

void printWavHeader(const WavHeader *header)
{
    printf("WavHeader:\n");
    printf("\tChunk ID: %p\n", header->chunkId);
    printf("\tChunk Size: %u\n", header->chunkSize);
    printf("\tFormat: %.4s\n", header->format);
    printf("\tSubchunk1 ID: %.4s\n", header->subchunk1Id);
    printf("\tSubchunk1 Size: %u\n", header->subchunk1Size);
    printf("\tAudio Format: %u\n", header->audioFormat);
    printf("\tNumber of Channels: %u\n", header->numChannels);
    printf("\tSample Rate: %u\n", header->sampleRate);
    printf("\tByte Rate: %u\n", header->byteRate);
    printf("\tBlock Align: %u\n", header->blockAlign);
    printf("\tBits Per Sample: %u\n", header->bitsPerSample);
    printf("\tSubchunk2 ID: %.4s\n", header->subchunk2Id);
    printf("\tSubchunk2 Size: %u\n", header->subchunk2Size);
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
    printWavHeader(&audioFile->header.wavHeader);
}