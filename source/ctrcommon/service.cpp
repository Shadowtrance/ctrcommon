#include <malloc.h>

#include <functional>
#include <map>

#include <3ds.h>
#include <stdio.h>
#include <string.h>

#include "service.hpp"

static u32* socBuffer;

Result fullFsInit() {
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

void fullFsExit() {
    sdmcExit();
    fsExit();
}

Result socInit() {
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

void socCleanup() {
    SOC_Shutdown();
    if(socBuffer != NULL) {
        free(socBuffer);
        socBuffer = NULL;
    }
}

std::map<std::string, std::function<void()>> services;
bool servicesInitialized = false;

bool serviceInit() {
    return (servicesInitialized = srvInit() == 0 && aptInit() == 0);
}

void serviceCleanup() {
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

bool serviceRequire(const std::string service) {
    if(!servicesInitialized) {
        return false;
    }

    if(services.find(service) != services.end()) {
        return true;
    }

    bool success = false;
    std::function<void()> cleanup = NULL;
    if(service.compare("gfx") == 0) {
        gfxInitDefault();
        success = true;
        cleanup = NULL;
    } else if(service.compare("hid") == 0) {
        success = hidInit(NULL) == 0;
        cleanup = &hidExit;
    } else if(service.compare("fs") == 0) {
        success = fullFsInit() == 0;
        cleanup = &fullFsExit;
    } else if(service.compare("am") == 0) {
        success = amInit() == 0;
        cleanup = &amExit;
    } else if(service.compare("ns") == 0) {
        success = nsInit() == 0;
        cleanup = &nsExit;
    } else if(service.compare("soc") == 0) {
        success = socInit() == 0;
        cleanup = &socCleanup;
    } else if(service.compare("csnd") == 0) {
        success = csndInit() == 0;
        cleanup = &csndExit;
    } else if(service.compare("console") == 0) {
        consoleDebugInit(debugDevice_3DMOO);
        success = true;
        cleanup = NULL;
    }

    if(success) {
        services[service] = cleanup;
    }

    return success;
}