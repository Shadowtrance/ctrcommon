#include "ctrcommon/common.hpp"
#include "service.hpp"

#include <3ds.h>

void* sound_alloc(u32 size) {
    return linearAlloc(size);
}

void sound_free(void* mem) {
    linearFree(mem);
}

bool sound_play(SoundChannel channel, SoundFormat format, u32 sampleRate, void* samples, u32 numSamples) {
    if(!service_require("csnd")) {
        return false;
    }

    Result res = csndPlaySound(channel, format | SOUND_ONE_SHOT, sampleRate, samples, samples, numSamples * (format == PCM16 ? 2 : 1));
    if(res != 0) {
        platform_set_error(service_parse_error((u32) res));
    }

    return res == 0;
}