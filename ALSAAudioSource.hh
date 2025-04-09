#ifndef _ALSA_AUDIO_SOURCE_HH
#define _ALSA_AUDIO_SOURCE_HH

#include "FramedSource.hh"
#include <alsa/asoundlib.h>
#include <opus/opus.h>

class ALSAAudioSource: public FramedSource {
public:
    static ALSAAudioSource* createNew(UsageEnvironment& env);

protected:
    ALSAAudioSource(UsageEnvironment& env);
    virtual ~ALSAAudioSource();

private:
    virtual void doGetNextFrame();
    void captureAudio();

    snd_pcm_t* pcmHandle;      // ALSA handle
    OpusEncoder* encoder;      // Opus encoder
    unsigned char* pcmBuffer;  // Raw PCM buffer
    unsigned char* opusBuffer; // Encoded Opus buffer
    unsigned int opusSize;
};

#endif
