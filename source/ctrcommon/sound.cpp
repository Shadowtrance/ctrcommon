#include "ctrcommon/sound.hpp"

#include "service.hpp"

#include <3ds.h>

void*soundAlloc(u32 size) {
    return linearAlloc(size);
}

void soundFree(void* mem) {
    linearFree(mem);
}

bool soundPlay(SoundChannel channel, SoundFormat format, u32 sampleRate, void* samples, u32 numSamples) {
    if(!serviceRequire("csnd")) {
        return false;
    }

    Result res = csndPlaySound(channel, format | SOUND_ONE_SHOT, sampleRate, samples, samples, numSamples * (format == PCM16 ? 2 : 1));
    if(res != 0) {
        platformSetError(serviceParseError((u32) res));
    }

    return res == 0;
}