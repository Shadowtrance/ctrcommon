#include "ctrcommon/common.hpp"

#include <3ds.h>

static bool soundInitialized;

bool sound_init() {
    if(!soundInitialized && csndInit() == 0) {
        soundInitialized = true;
    }

    return soundInitialized;
}

void sound_cleanup() {
    if(soundInitialized) {
        csndExit();
        soundInitialized = false;
    }
}

void* sound_alloc(u32 size) {
    return linearAlloc(size);
}

void sound_free(void* mem) {
    linearFree(mem);
}

bool sound_play(SoundChannel channel, SoundFormat format, u32 sampleRate, void* samples, u32 numSamples) {
    if(!sound_init()) {
        return false;
    }

    csndPlaySound(channel, format | SOUND_ONE_SHOT, sampleRate, samples, samples, numSamples * (format == PCM16 ? 2 : 1));
    return true;
}