#include "service.hpp"

#include <malloc.h>
#include <stdio.h>

#include <functional>
#include <map>

#include <3ds.h>

#define GET_BITS(v, s, e) (((v) >> (s)) & ((1 << ((e) - (s) + 1)) - 1))

static u32* socBuffer;

Result fs_init() {
    Result fsResult = fsInit();
    if(fsResult != 0) {
        return fsResult;
    }

    Result sdmcResult = sdmcInit();
    if(sdmcResult != 0) {
        return sdmcResult;
    }

    return 0;
}

void fs_exit() {
    sdmcExit();
    fsExit();
}

Result soc_init() {
    socBuffer = (u32*) memalign(0x1000, 0x100000);
    if(socBuffer == NULL) {
        return -1;
    }

    Result socResult = SOC_Initialize(socBuffer, 0x100000);
    if(socResult != 0) {
        free(socBuffer);
        socBuffer = NULL;
        return socResult;
    }

    return 0;
}

void soc_cleanup() {
    SOC_Shutdown();
    if(socBuffer != NULL) {
        free(socBuffer);
        socBuffer = NULL;
    }
}

static std::map<std::string, std::function<void()>> services;
static bool servicesInitialized = false;

bool service_init() {
    return (servicesInitialized = srvInit() == 0 && aptInit() == 0);
}

void service_cleanup() {
    if(!servicesInitialized) {
        return;
    }

    for(std::map<std::string, std::function<void()>>::iterator it = services.begin(); it != services.end(); it++) {
        if((*it).second != NULL) {
            (*it).second();
        }
    }

    services.clear();
    aptExit();
    srvExit();
}

bool service_require(const std::string service) {
    if(!servicesInitialized) {
        return false;
    }

    if(services.find(service) != services.end()) {
        return true;
    }

    Result result = 0;
    std::function<void()> cleanup = NULL;
    if(service.compare("gfx") == 0) {
        gfxInitDefault();
        result = 0;
        cleanup = &gfxExit;
    } else if(service.compare("hid") == 0) {
        result = hidInit(NULL);
        cleanup = &hidExit;
    } else if(service.compare("fs") == 0) {
        result = fs_init();
        cleanup = &fs_exit;
    } else if(service.compare("am") == 0) {
        result = amInit();
        cleanup = &amExit;
    } else if(service.compare("ns") == 0) {
        result = nsInit();
        cleanup = &nsExit;
    } else if(service.compare("soc") == 0) {
        result = soc_init();
        cleanup = &soc_cleanup;
    } else if(service.compare("csnd") == 0) {
        result = csndInit();
        cleanup = &csndExit;
    } else if(service.compare("console") == 0) {
        consoleDebugInit(debugDevice_3DMOO);
        result = 0;
        cleanup = NULL;
    }

    if(result == 0) {
        services[service] = cleanup;
    } else {
        platform_set_error(service_parse_error((u32) result));
    }

    return result == 0;
}

Error service_parse_error(u32 error) {
    Error err;

    u32 module = GET_BITS(error, 10, 17);
    if(module == 79) {
        module = MODULE_RO;
    }

    err.raw = error;
    err.module = (ErrorModule) module;
    err.level = (ErrorLevel) GET_BITS(error, 27, 31);
    err.summary = (ErrorSummary) GET_BITS(error, 21, 26);
    err.description = (ErrorDescription) GET_BITS(error, 0, 9);

    return err;
}