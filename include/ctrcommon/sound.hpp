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

void* sound_alloc(u32 size);
void sound_free(void* mem);
bool sound_play(SoundChannel channel, SoundFormat format, u32 sampleRate, void* samples, u32 numSamples);

#endif
