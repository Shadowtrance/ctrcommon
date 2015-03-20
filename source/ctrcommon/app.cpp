#include "ctrcommon/app.hpp"

#include "service.hpp"

#include <sys/errno.h>
#include <sys/unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <iomanip>
#include <sstream>

#include <3ds.h>

u8 appMediatypeToByte(MediaType mediaType) {
    return mediaType == NAND ? mediatype_NAND : mediatype_SDMC;
}

AppPlatform appPlatformFromId(u16 id) {
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

AppCategory appCategoryFromId(u16 id) {
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

const std::string appGetResultString(AppResult result) {
    std::stringstream resultMsg;
    if(result == APP_SUCCESS) {
        resultMsg << "Operation succeeded.";
    } else if(result == APP_PROCESS_CLOSING) {
        resultMsg << "Process closing.";
    } else if(result == APP_OPERATION_CANCELLED) {
        resultMsg << "Operation cancelled.";
    } else if(result == APP_AM_INIT_FAILED) {
        resultMsg << "Could not initialize AM service.";
    } else if(result == APP_NS_INIT_FAILED) {
        resultMsg << "Could not initialize NS service.";
    } else if(result == APP_IO_ERROR) {
        resultMsg << "I/O Error." << "\n" << strerror(errno);
    } else if(result == APP_OPEN_FILE_FAILED) {
        resultMsg << "Could not open file." << "\n" << strerror(errno);
    } else if(result == APP_BEGIN_INSTALL_FAILED) {
        resultMsg << "Could not begin installation." << "\n" << platformGetErrorString(platformGetError());
    } else if(result == APP_INSTALL_ERROR) {
        resultMsg << "Could not install app." << "\n" << platformGetErrorString(platformGetError());
    } else if(result == APP_FINALIZE_INSTALL_FAILED) {
        resultMsg << "Could not finalize installation." << "\n" << platformGetErrorString(platformGetError());
    } else if(result == APP_DELETE_FAILED) {
        resultMsg << "Could not delete app." << "\n" << platformGetErrorString(platformGetError());
    } else if(result == APP_LAUNCH_FAILED) {
        resultMsg << "Could not launch app." << "\n" << platformGetErrorString(platformGetError());
    } else {
        resultMsg << "Unknown error.";
    }

    return resultMsg.str();
}

const std::string appGetPlatformName(AppPlatform platform) {
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

const std::string appGetCategoryName(AppCategory category) {
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

std::vector<App> appList(MediaType mediaType) {
    std::vector<App> titles;
    if(!serviceRequire("am")) {
        return titles;
    }

    u32 titleCount;
    Result titleCountResult = AM_GetTitleCount(appMediatypeToByte(mediaType), &titleCount);
    if(titleCountResult != 0) {
        platformSetError(serviceParseError((u32) titleCountResult));
        return titles;
    }

    u64 titleIds[titleCount];
    Result titleListResult = AM_GetTitleIdList(appMediatypeToByte(mediaType), titleCount, titleIds);
    if(titleListResult != 0) {
        platformSetError(serviceParseError((u32) titleListResult));
        return titles;
    }

    for(u32 i = 0; i < titleCount; i++) {
        u64 titleId = titleIds[i];
        App app;
        app.titleId = titleId;
        app.uniqueId = ((u32*) &titleId)[0];
        AM_GetTitleProductCode(appMediatypeToByte(mediaType), titleId, app.productCode);
        if(strcmp(app.productCode, "") == 0) {
            strcpy(app.productCode, "<N/A>");
        }

        app.mediaType = mediaType;
        app.platform = appPlatformFromId(((u16 *) &titleId)[3]);
        app.category = appCategoryFromId(((u16 *) &titleId)[2]);

        titles.push_back(app);
    }

    return titles;
}

AppResult appInstallFile(MediaType mediaType, const std::string path, std::function<bool(int progress)> onProgress) {
    if(!serviceRequire("am")) {
        return APP_AM_INIT_FAILED;
    }

    FILE* fd = fopen(path.c_str(), "r");
    if(!fd) {
        return APP_OPEN_FILE_FAILED;
    }

    fseek(fd, 0, SEEK_END);
    u64 size = (u64) ftell(fd);
    fseek(fd, 0, SEEK_SET);

    AppResult ret = appInstall(mediaType, fd, size, onProgress);

    fclose(fd);
    return ret;
}

AppResult appInstall(MediaType mediaType, FILE* fd, u64 size, std::function<bool(int progress)> onProgress) {
    if(!serviceRequire("am")) {
        return APP_AM_INIT_FAILED;
    }

    if(onProgress != NULL) {
        onProgress(0);
    }

    Handle ciaHandle;
    Result startResult = AM_StartCiaInstall(appMediatypeToByte(mediaType), &ciaHandle);
    if(startResult != 0) {
        platformSetError(serviceParseError((u32) startResult));
        return APP_BEGIN_INSTALL_FAILED;
    }

    u32 bufSize = 1024 * 16; // 16KB
    void* buf = malloc(bufSize);
    bool cancelled = false;
    u64 pos = 0;
    while(platformIsRunning()) {
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
            Result writeResult = FSFILE_Write(ciaHandle, NULL, pos, buf, (u32) bytesRead, FS_WRITE_NOFLUSH);
            if(writeResult != 0) {
                AM_CancelCIAInstall(&ciaHandle);
                platformSetError(serviceParseError((u32) writeResult));
                return APP_INSTALL_ERROR;
            }

            pos += bytesRead;
        }
    }

    free(buf);

    if(cancelled) {
        return APP_OPERATION_CANCELLED;
    }

    if(!platformIsRunning()) {
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

    Result finishResult = AM_FinishCiaInstall(appMediatypeToByte(mediaType), &ciaHandle);
    if(finishResult != 0) {
        platformSetError(serviceParseError((u32) finishResult));
        return APP_FINALIZE_INSTALL_FAILED;
    }

    return APP_SUCCESS;
}

AppResult appDelete(App app) {
    if(!serviceRequire("am")) {
        return APP_AM_INIT_FAILED;
    }

    Result res = AM_DeleteAppTitle(appMediatypeToByte(app.mediaType), app.titleId);
    if(res != 0) {
        platformSetError(serviceParseError((u32) res));
        return APP_DELETE_FAILED;
    }

    return APP_SUCCESS;
}

AppResult appLaunch(App app) {
    if(!serviceRequire("ns")) {
        return APP_NS_INIT_FAILED;
    }

    Result res = NS_RebootToTitle(app.mediaType, app.titleId);
    if(res != 0) {
        platformSetError(serviceParseError((u32) res));
        return APP_LAUNCH_FAILED;
    }

    return APP_SUCCESS;
}