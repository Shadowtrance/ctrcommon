#ifndef __CTRCOMMON_APP_HPP__
#define __CTRCOMMON_APP_HPP__

#include "ctrcommon/fs.hpp"

#include <functional>
#include <string>
#include <vector>

typedef enum {
    WII,
    DSI,
    THREEDS,
    WIIU,
    UNKNOWN_PLATFORM
} AppPlatform;

// TODO: verify categories.
typedef enum {
    APP,
    DLC,
    PATCH,
    SYSTEM,
    TWL
} AppCategory;

typedef struct {
    u64 titleId;
    u32 uniqueId;
    char productCode[16];
    MediaType mediaType;
    AppPlatform platform;
    AppCategory category;
} App;

typedef enum {
    APP_SUCCESS,
    APP_PROCESS_CLOSING,
    APP_OPERATION_CANCELLED,
    APP_AM_INIT_FAILED,
    APP_NS_INIT_FAILED,
    APP_IO_ERROR,
    APP_OPEN_FILE_FAILED,
    APP_BEGIN_INSTALL_FAILED,
    APP_INSTALL_ERROR,
    APP_FINALIZE_INSTALL_FAILED,
    APP_DELETE_FAILED,
    APP_LAUNCH_FAILED
} AppResult;

const std::string app_get_result_string(AppResult result);
const std::string app_get_platform_name(AppPlatform platform);
const std::string app_get_category_name(AppCategory category);
std::vector<App> app_list(MediaType mediaType);
AppResult app_install_file(MediaType mediaType, const std::string path, std::function<bool(int progress)> onProgress);
AppResult app_install(MediaType mediaType, FILE* fd, u64 size, std::function<bool(int progress)> onProgress);
AppResult app_delete(App app);
AppResult app_launch(App app);

#endif