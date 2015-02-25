#include "ctrcommon/common.hpp"
#include "service.hpp"

#include <sys/errno.h>
#include <sys/unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <iomanip>
#include <sstream>

#include <3ds.h>

u8 app_mediatype_to_byte(MediaType mediaType) {
    return mediaType == NAND ? mediatype_NAND : mediatype_SDMC;
}

AppPlatform app_platform_from_id(u16 id) {
    switch(id) {
        case 1:
            return WII;
        case 3:
            return DSI;
        case 4:
            return THREEDS;
        case 5:
            return WIIU;
        default:
            return UNKNOWN_PLATFORM;
    }
}

AppCategory app_category_from_id(u16 id) {
    if((id & 0x2) == 0x2) {
        return DLC;
    } else if((id & 0x6) == 0x6) {
        return PATCH;
    } else if((id & 0x10) == 0x10) {
        return SYSTEM;
    } else if((id & 0x8000) == 0x8000) {
        return TWL;
    }

    return APP;
}

const std::string app_get_result_string(AppResult result) {
    std::stringstream resultMsg;
    if(result == APP_SUCCESS) {
        resultMsg << "Operation succeeded.";
    } else if(result == APP_AM_INIT_FAILED) {
        resultMsg << "Could not initialize AM service.";
    } else if(result == APP_NS_INIT_FAILED) {
        resultMsg << "Could not initialize NS service.";
    } else if(result == APP_OPERATION_CANCELLED) {
        resultMsg << "Operation cancelled.";
    } else if(result == APP_BEGIN_INSTALL_FAILED) {
        resultMsg << "Could not begin installation: Error 0x" << std::hex << errno;
    } else if(result == APP_IO_ERROR) {
        resultMsg << "I/O Error 0x" << std::hex << errno;
    } else if(result == APP_FINALIZE_INSTALL_FAILED) {
        resultMsg << "Could not finalize installation: Error 0x" << std::hex << errno;
    } else if(result == APP_OPEN_FILE_FAILED) {
        resultMsg << "Could not open file: Error 0x" << std::hex << errno;
    } else if(result == APP_DELETE_FAILED) {
        resultMsg << "Could not delete app: Error 0x" << std::hex << errno;
    } else if(result == APP_LAUNCH_FAILED) {
        resultMsg << "Could not launch app: Error 0x" << std::hex << errno;
    } else if(result == APP_PROCESS_CLOSING) {
        resultMsg << "Process closing.";
    } else {
        resultMsg << "Unknown error.";
    }

    return resultMsg.str();
}

const std::string app_get_platform_name(AppPlatform platform) {
    switch(platform) {
        case WII:
            return "Wii";
        case DSI:
            return "DSi";
        case THREEDS:
            return "3DS";
        case WIIU:
            return "Wii U";
        default:
            return "Unknown";
    }
}

const std::string app_get_category_name(AppCategory category) {
    switch(category) {
        case APP:
            return "App";
        case DLC:
            return "DLC";
        case PATCH:
            return "Patch";
        case SYSTEM:
            return "System";
        case TWL:
            return "TWL";
        default:
            return "Unknown";
    }
}

std::vector<App> app_list(MediaType mediaType) {
    std::vector<App> titles;
    if(!serviceRequire("am")) {
        return titles;
    }

    u32 titleCount;
    if(AM_GetTitleCount(app_mediatype_to_byte(mediaType), &titleCount) != 0) {
        return titles;
    }

    u64 titleIds[titleCount];
    if(AM_GetTitleList(app_mediatype_to_byte(mediaType), titleCount, titleIds) != 0) {
        return titles;
    }

    for(u32 i = 0; i < titleCount; i++) {
        u64 titleId = titleIds[i];
        App app;
        app.titleId = titleId;
        app.uniqueId = ((u32 *) &titleId)[0];
        AM_GetTitleProductCode(app_mediatype_to_byte(mediaType), titleId, app.productCode);
        if(strcmp(app.productCode, "") == 0) {
            strcpy(app.productCode, "<N/A>");
        }

        app.mediaType = mediaType;
        app.platform = app_platform_from_id(((u16 *) &titleId)[3]);
        app.category = app_category_from_id(((u16 *) &titleId)[2]);

        titles.push_back(app);
    }

    return titles;
}

AppResult app_install_file(MediaType mediaType, const std::string path, std::function<bool(int progress)> onProgress) {
    errno = 0;
    if(!serviceRequire("am")) {
        return APP_AM_INIT_FAILED;
    }

    FILE *fd = fopen(path.c_str(), "r");
    if(!fd) {
        return APP_OPEN_FILE_FAILED;
    }

    fseek(fd, 0, SEEK_END);
    u64 size = (u64) ftell(fd);
    fseek(fd, 0, SEEK_SET);

    AppResult ret = app_install(mediaType, fd, size, onProgress);

    fclose(fd);
    return ret;
}

AppResult app_install(MediaType mediaType, FILE* fd, u64 size, std::function<bool(int progress)> onProgress) {
    errno = 0;
    if(!serviceRequire("am")) {
        return APP_AM_INIT_FAILED;
    }

    if(onProgress != NULL) {
        onProgress(0);
    }

    Handle ciaHandle;
    int startRet = AM_StartCiaInstall(app_mediatype_to_byte(mediaType), &ciaHandle);
    if(startRet != 0) {
        errno = startRet;
        return APP_BEGIN_INSTALL_FAILED;
    }

    u32 bufSize = 1024 * 16; // 16KB
    void *buf = malloc(bufSize);
    bool cancelled = false;
    u64 pos = 0;
    while(platform_is_running()) {
        if(onProgress != NULL && !onProgress(size != 0 ? (int) ((pos / (float) size) * 100) : 0)) {
            AM_CancelCIAInstall(&ciaHandle);
            cancelled = true;
            break;
        }

        u64 bytesRead = fread(buf, 1, bufSize, fd);
        if(bytesRead == 0) {
            if((errno != EAGAIN && errno != EWOULDBLOCK) || (size != 0 && pos == size)) {
                break;
            }
        } else {
            FSFILE_Write(ciaHandle, NULL, pos, buf, (u32) bytesRead, FS_WRITE_NOFLUSH);
            pos += bytesRead;
        }
    }

    free(buf);

    if(cancelled) {
        return APP_OPERATION_CANCELLED;
    }

    if(!platform_is_running()) {
        AM_CancelCIAInstall(&ciaHandle);
        return APP_PROCESS_CLOSING;
    }

    if(size != 0 && pos != size) {
        AM_CancelCIAInstall(&ciaHandle);
        return APP_IO_ERROR;
    }

    if(onProgress != NULL) {
        onProgress(100);
    }

    Result res = AM_FinishCiaInstall(app_mediatype_to_byte(mediaType), &ciaHandle);
    if(res != 0 && (u32) res != 0xC8A044DC) { // Happens when already installed, but seems to have succeeded anyway...
        errno = res;
        return APP_FINALIZE_INSTALL_FAILED;
    }

    return APP_SUCCESS;
}

AppResult app_delete(App app) {
    errno = 0;
    if(!serviceRequire("am")) {
        return APP_AM_INIT_FAILED;
    }

    Result res = AM_DeleteAppTitle(app_mediatype_to_byte(app.mediaType), app.titleId);
    if(res != 0) {
        errno = res;
        return APP_DELETE_FAILED;
    }

    return APP_SUCCESS;
}

AppResult app_launch(App app) {
    errno = 0;
    if(!serviceRequire("ns")) {
        return APP_NS_INIT_FAILED;
    }

    Result res = NS_RebootToTitle(app.mediaType, app.titleId);
    if(res != 0) {
        errno = res;
        return APP_LAUNCH_FAILED;
    }

    return APP_SUCCESS;
}