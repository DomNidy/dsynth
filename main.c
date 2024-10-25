#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <stdalign.h>

#include "audiofile.h"
#include "freqs.h"

int main()
{
    AudioFile *audioFile = createWavFile("audio.wav", 44100, 16, 1, 90.0);

    // Example measure
    // Bar 0
    writeSineWave(audioFile, FREQ_C4, audioFile->samplesPerBeat / 3);
    writeSineWave(audioFile, FREQ_G3, audioFile->samplesPerBeat / 3);
    writeSineWave(audioFile, FREQ_E4, audioFile->samplesPerBeat / 3);

    writeSineWave(audioFile, FREQ_B4, audioFile->samplesPerBeat / 3);
    writeSineWave(audioFile, FREQ_G3, audioFile->samplesPerBeat / 3);
    writeSineWave(audioFile, FREQ_E4, audioFile->samplesPerBeat / 3);

    writeSineWave(audioFile, FREQ_A4, audioFile->samplesPerBeat / 3);
    writeSineWave(audioFile, FREQ_G3, audioFile->samplesPerBeat / 3);
    writeSineWave(audioFile, FREQ_D4, audioFile->samplesPerBeat / 3);

    writeSineWave(audioFile, FREQ_A4, audioFile->samplesPerBeat / 3);
    writeSineWave(audioFile, FREQ_C5, audioFile->samplesPerBeat / 3);
    writeSineWave(audioFile, FREQ_G5, audioFile->samplesPerBeat / 3);

    // Bar 1
    writeSineWave(audioFile, FREQ_C4, audioFile->samplesPerBeat / 3);
    writeSineWave(audioFile, FREQ_G3, audioFile->samplesPerBeat / 3);
    writeSineWave(audioFile, FREQ_E4, audioFile->samplesPerBeat / 3);

    writeSineWave(audioFile, FREQ_B4, audioFile->samplesPerBeat / 3);
    writeSineWave(audioFile, FREQ_G3, audioFile->samplesPerBeat / 3);
    writeSineWave(audioFile, FREQ_E4, audioFile->samplesPerBeat / 3);

    writeSineWave(audioFile, FREQ_A4, audioFile->samplesPerBeat / 3);
    writeSineWave(audioFile, FREQ_G3, audioFile->samplesPerBeat / 3);
    writeSineWave(audioFile, FREQ_D4, audioFile->samplesPerBeat / 3);

    writeSineWave(audioFile, FREQ_A4, audioFile->samplesPerBeat / 3);
    writeSineWave(audioFile, FREQ_C5, audioFile->samplesPerBeat / 3);
    writeSineWave(audioFile, FREQ_G5, audioFile->samplesPerBeat / 3);

    // Bar 2
    writeSineWave(audioFile, FREQ_C4, audioFile->samplesPerBeat / 3);
    writeSineWave(audioFile, FREQ_A4, audioFile->samplesPerBeat / 3);
    writeSineWave(audioFile, FREQ_F4, audioFile->samplesPerBeat / 3);

    writeSineWave(audioFile, FREQ_B4, audioFile->samplesPerBeat / 3);
    writeSineWave(audioFile, FREQ_A4, audioFile->samplesPerBeat / 3);
    writeSineWave(audioFile, FREQ_E4, audioFile->samplesPerBeat / 3);

    writeSineWave(audioFile, FREQ_A4, audioFile->samplesPerBeat / 3);
    writeSineWave(audioFile, FREQ_G3, audioFile->samplesPerBeat / 3);
    writeSineWave(audioFile, FREQ_D4, audioFile->samplesPerBeat / 3);

    writeSineWave(audioFile, FREQ_A4, audioFile->samplesPerBeat / 3);
    writeSineWave(audioFile, FREQ_C5, audioFile->samplesPerBeat / 3);
    writeSineWave(audioFile, FREQ_G5, audioFile->samplesPerBeat / 3);

    // Bar 3
    writeSineWave(audioFile, FREQ_C4, audioFile->samplesPerBeat / 3);
    writeSineWave(audioFile, FREQ_A4, audioFile->samplesPerBeat / 3);
    writeSineWave(audioFile, FREQ_F4, audioFile->samplesPerBeat / 3);

    writeSineWave(audioFile, FREQ_B4, audioFile->samplesPerBeat / 3);
    writeSineWave(audioFile, FREQ_A4, audioFile->samplesPerBeat / 3);
    writeSineWave(audioFile, FREQ_E4, audioFile->samplesPerBeat / 3);

    writeSineWave(audioFile, FREQ_A4, audioFile->samplesPerBeat / 3);
    writeSineWave(audioFile, FREQ_G3, audioFile->samplesPerBeat / 3);
    writeSineWave(audioFile, FREQ_D4, audioFile->samplesPerBeat / 3);

    writeSineWave(audioFile, FREQ_A4, audioFile->samplesPerBeat / 3);
    writeSineWave(audioFile, FREQ_C5, audioFile->samplesPerBeat / 3);
    writeSineWave(audioFile, FREQ_G5, audioFile->samplesPerBeat / 3);

    printAudioFile(audioFile);

    // Close the file
    fclose(audioFile->audioFile);
    return 0;
}
