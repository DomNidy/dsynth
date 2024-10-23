#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <stdalign.h>

#include "audiofile.h"
#include "freqs.h"

int main()
{
    AudioFile *audioFile = createWavFile("audio.wav", 44100, 16, 1, 140.0);

    // write 2 bars of a sine wave (2 bars assuming we're in 4:4)
    writeSineWave(audioFile, FREQ_C5, 4 * audioFile->samplesPerBeat);
    writeSineWave(audioFile, FREQ_G5, 4 * audioFile->samplesPerBeat);

    printf("Samples written: %d\n", audioFile->samplesWritten);
    printf("After writing\n\n");

    printAudioFile(audioFile);

    // Close the file
    fclose(audioFile->audioFile);
    return 0;
}
