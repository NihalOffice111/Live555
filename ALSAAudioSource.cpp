#include "ALSAAudioSource.hh"
#include <string.h>

ALSAAudioSource* ALSAAudioSource::createNew(UsageEnvironment& env) {
    return new ALSAAudioSource(env);
}

ALSAAudioSource::ALSAAudioSource(UsageEnvironment& env)
    : FramedSource(env), pcmHandle(NULL), encoder(NULL), pcmBuffer(NULL), opusBuffer(NULL), opusSize(0) {
    // Open ALSA device
    snd_pcm_open(&pcmHandle, "default", SND_PCM_STREAM_CAPTURE, 0);
    snd_pcm_hw_params_t* params;
    snd_pcm_hw_params_alloca(&params);
    snd_pcm_hw_params_any(pcmHandle, params);
    snd_pcm_hw_params_set_access(pcmHandle, params, SND_PCM_ACCESS_RW_INTERLEAVED);
    snd_pcm_hw_params_set_format(pcmHandle, params, SND_PCM_FORMAT_S16_LE);
    unsigned int rate = 48000;
    snd_pcm_hw_params_set_rate_near(pcmHandle, params, &rate, 0);
    snd_pcm_hw_params_set_channels(pcmHandle, params, 2);
    snd_pcm_hw_params(pcmHandle, params);

    // Initialize Opus
    int err;
    encoder = opus_encoder_create(48000, 2, OPUS_APPLICATION_VOIP, &err);
    pcmBuffer = new unsigned char[480 * 2 * 2]; // 20ms at 48kHz, stereo, 16-bit
    opusBuffer = new unsigned char[4000];       // Max Opus frame size
}

ALSAAudioSource::~ALSAAudioSource() {
    snd_pcm_close(pcmHandle);
    opus_encoder_destroy(encoder);
    delete[] pcmBuffer;
    delete[] opusBuffer;
}

void ALSAAudioSource::doGetNextFrame() {
    captureAudio();
    if (opusSize > 0) {
        envir() << "Sending video frame, size: " << opusSize << "\n"; // Debug
        if (opusSize > fMaxSize) {
            fFrameSize = fMaxSize;
            fNumTruncatedBytes = opusSize - fMaxSize;
        } else {
            fFrameSize = opusSize;
        }
        memcpy(fTo, opusBuffer, fFrameSize);
        gettimeofday(&fPresentationTime, NULL);
        FramedSource::afterGetting(this);
    }
    else{
        envir() << "No audio frame captured\n"; // Debug
    }
}

void ALSAAudioSource::captureAudio() {
    snd_pcm_readi(pcmHandle, pcmBuffer, 480); // 20ms frames
    opusSize = opus_encode(encoder, (opus_int16*)pcmBuffer, 480, opusBuffer, 4000);
}




/*
g++ -o RTPStreamer RTPStreamer.cpp V4L2VideoSource.cpp ALSAAudioSource.cpp -I../liveMedia/include -I../groupsock/include -I../BasicUsageEnvironment/include -I../UsageEnvironment/include -L../liveMedia -L../groupsock -L../BasicUsageEnvironment -L../UsageEnvironment -lliveMedia -lgroupsock -lBasicUsageEnvironment -lUsageEnvironment -lv4l2 -lturbojpeg -lasound -lopus -lssl -lcrypto

*/