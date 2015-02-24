#include "ctrcommon/common.hpp"
#include "service.hpp"

#include <stdio.h>

#include <3ds.h>

bool platform_init() {
    return serviceInit() && serviceRequire("fs");
}

void platform_cleanup() {
    serviceCleanup();
}

bool platform_is_running() {
    return aptMainLoop();
}

u32 platform_get_device_id() {
    if(!serviceRequire("am")) {
        return 0;
    }

    u32 deviceId;
    if(AM_GetDeviceId(&deviceId) != 0) {
        return 0;
    }
    
    return deviceId;
}

u64 platform_get_time() {
    return osGetTime();
}

void platform_delay(int ms) {
    svcSleepThread(ms * 1000000);
}

void platform_printf(const char *format, ...) {
    if(!serviceRequire("console")) {
        return;
    }

    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
}