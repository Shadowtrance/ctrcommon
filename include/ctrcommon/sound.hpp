#ifndef __CTRCOMMON_SOUND_HPP__
#define __CTRCOMMON_SOUND_HPP__

#include "ctrcommon/types.hpp"

typedef enum {
    CHANNEL_LEFT = 8,
    CHANNEL_RIGHT = 9
} SoundChannel;

typedef enum {
    PCM8 = 0,
    PCM16 = 4096
} SoundFormat;

void* soundAlloc(u32 size);
void soundFree(void* mem);
bool soundPlay(SoundChannel channel, SoundFormat format, u32 sampleRate, void* samples, u32 numSamples);

#endif
