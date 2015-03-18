#include "ctrcommon/common.hpp"
#include "service.hpp"

#include <malloc.h>
#include <stdio.h>
#include <string.h>

#include <sstream>

#include <3ds.h>

static Error* currentError;

bool platform_init() {
    return service_init() && service_require("fs") && service_require("gfx");
}

void platform_cleanup() {
    service_cleanup();
}

bool platform_is_running() {
    return aptMainLoop();
}

bool platform_is_ninjhax() {
    Result result = hbInit();
    if(result == 0) {
        hbExit();
    }

    return result == 0;
}

u32 platform_get_device_id() {
    if(!service_require("am")) {
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
    char buffer[256];

    va_list args;
    va_start(args, format);
    vsnprintf(buffer, 256, format, args);
    va_end(args);

    svcOutputDebugString(buffer, strlen(buffer));
}

Error platform_get_error() {
    Error error = *currentError;
    if(currentError != NULL) {
        free(currentError);
        currentError = NULL;
    }

    return error;
}

void platform_set_error(Error error) {
    if(currentError == NULL) {
        currentError = (Error*) malloc(sizeof(Error));
    }

    *currentError = error;
}

std::string platform_get_error_string(Error error) {
    std::stringstream result;

    result << "Raw Error: 0x" << std::hex << error.raw << "\n";

    result << "Module: ";
    switch(error.module) {
        case MODULE_COMMON:
            result << "Common";
            break;
        case MODULE_KERNEL:
            result << "Kernel";
            break;
        case MODULE_UTIL:
            result << "Util";
            break;
        case MODULE_FILE_SERVER:
            result << "File server";
            break;
        case MODULE_LOADER_SERVER:
            result << "Loader server";
            break;
        case MODULE_TCB:
            result << "TCB";
            break;
        case MODULE_OS:
            result << "OS";
            break;
        case MODULE_DBG:
            result << "DBG";
            break;
        case MODULE_DMNT:
            result << "DMNT";
            break;
        case MODULE_PDN:
            result << "PDN";
            break;
        case MODULE_GX:
            result << "GX";
            break;
        case MODULE_I2C:
            result << "I2C";
            break;
        case MODULE_GPIO:
            result << "GPIO";
            break;
        case MODULE_DD:
            result << "DD";
            break;
        case MODULE_CODEC:
            result << "CODEC";
            break;
        case MODULE_SPI:
            result << "SPI";
            break;
        case MODULE_PXI:
            result << "PXI";
            break;
        case MODULE_FS:
            result << "FS";
            break;
        case MODULE_DI:
            result << "DI";
            break;
        case MODULE_HID:
            result << "HID";
            break;
        case MODULE_CAM:
            result << "CAM";
            break;
        case MODULE_PI:
            result << "PI";
            break;
        case MODULE_PM:
            result << "PM";
            break;
        case MODULE_PM_LOW:
            result << "PM_LOW";
            break;
        case MODULE_FSI:
            result << "FSI";
            break;
        case MODULE_SRV:
            result << "SRV";
            break;
        case MODULE_NDM:
            result << "NDM";
            break;
        case MODULE_NWM:
            result << "NWM";
            break;
        case MODULE_SOC:
            result << "SOC";
            break;
        case MODULE_LDR:
            result << "LDR";
            break;
        case MODULE_ACC:
            result << "ACC";
            break;
        case MODULE_ROMFS:
            result << "RomFS";
            break;
        case MODULE_AM:
            result << "AM";
            break;
        case MODULE_HIO:
            result << "HIO";
            break;
        case MODULE_UPDATER:
            result << "Updater";
            break;
        case MODULE_MIC:
            result << "MIC";
            break;
        case MODULE_FND:
            result << "FND";
            break;
        case MODULE_MP:
            result << "MP";
            break;
        case MODULE_MPWL:
            result << "MPWL";
            break;
        case MODULE_AC:
            result << "AC";
            break;
        case MODULE_HTTP:
            result << "HTTP";
            break;
        case MODULE_DSP:
            result << "DSP";
            break;
        case MODULE_SND:
            result << "SND";
            break;
        case MODULE_DLP:
            result << "DLP";
            break;
        case MODULE_HIO_LOW:
            result << "HIO_LOW";
            break;
        case MODULE_CSND:
            result << "CSND";
            break;
        case MODULE_SSL:
            result << "SSL";
            break;
        case MODULE_AM_LOW:
            result << "AM_LOW";
            break;
        case MODULE_NEX:
            result << "NEX";
            break;
        case MODULE_FRIENDS:
            result << "Friends";
            break;
        case MODULE_RDT:
            result << "RDT";
            break;
        case MODULE_APPLET:
            result << "Applet";
            break;
        case MODULE_NIM:
            result << "NIM";
            break;
        case MODULE_PTM:
            result << "PTM";
            break;
        case MODULE_MIDI:
            result << "MIDI";
            break;
        case MODULE_MC:
            result << "MC";
            break;
        case MODULE_SWC:
            result << "SWC";
            break;
        case MODULE_FATFS:
            result << "FatFS";
            break;
        case MODULE_NGC:
            result << "NGC";
            break;
        case MODULE_CARD:
            result << "CARD";
            break;
        case MODULE_CARDNOR:
            result << "CARDNOR";
            break;
        case MODULE_SDMC:
            result << "SDMC";
            break;
        case MODULE_BOSS:
            result << "BOSS";
            break;
        case MODULE_DBM:
            result << "DBM";
            break;
        case MODULE_CONFIG:
            result << "Config";
            break;
        case MODULE_PS:
            result << "PS";
            break;
        case MODULE_CEC:
            result << "CEC";
            break;
        case MODULE_IR:
            result << "IR";
            break;
        case MODULE_UDS:
            result << "UDS";
            break;
        case MODULE_PL:
            result << "PL";
            break;
        case MODULE_CUP:
            result << "CUP";
            break;
        case MODULE_GYROSCOPE:
            result << "Gyroscope";
            break;
        case MODULE_MCU:
            result << "MCU";
            break;
        case MODULE_NS:
            result << "NS";
            break;
        case MODULE_NEWS:
            result << "News";
            break;
        case MODULE_RO:
            result << "RO";
            break;
        case MODULE_GD:
            result << "GD";
            break;
        case MODULE_CARD_SPI:
            result << "Card SPI";
            break;
        case MODULE_EC:
            result << "EC";
            break;
        case MODULE_WEB_BROWSER:
            result << "Web browser";
            break;
        case MODULE_TEST:
            result << "Test";
            break;
        case MODULE_ENC:
            result << "ENC";
            break;
        case MODULE_PIA:
            result << "PIA";
            break;
        case MODULE_MVD:
            result << "MVD";
            break;
        case MODULE_QTM:
            result << "QTM";
            break;
        case MODULE_APPLICATION:
            result << "Application";
            break;
        default:
            result << "Invalid module";
            break;
    }

    result << " (0x" << std::hex << error.module << ")" << "\n";

    result << "Level: ";
    switch(error.level) {
        case LEVEL_SUCCESS:
            result << "Success";
            break;
        case LEVEL_INFO:
            result << "Info";
            break;
        case LEVEL_STATUS:
            result << "Status";
            break;
        case LEVEL_TEMPORARY:
            result << "Temporary";
            break;
        case LEVEL_PERMANENT:
            result << "Permanent";
            break;
        case LEVEL_USAGE:
            result << "Usage";
            break;
        case LEVEL_REINITIALIZE:
            result << "Reinitialize";
            break;
        case LEVEL_RESET:
            result << "Reset";
            break;
        case LEVEL_FATAL:
            result << "Fatal";
            break;
        default:
            result << "Invalid level";
            break;
    }

    result << " (0x" << std::hex << error.level << ")" << "\n";

    result << "Summary: ";
    switch(error.summary) {
        case SUMMARY_SUCCESS:
            result << "Success";
            break;
        case SUMMARY_NOTHING_HAPPENED:
            result << "Nothing happened";
            break;
        case SUMMARY_WOULD_BLOCK:
            result << "Would block";
            break;
        case SUMMARY_OUT_OF_RESOURCE:
            result << "Out of resource";
            break;
        case SUMMARY_NOT_FOUND:
            result << "Not found";
            break;
        case SUMMARY_INVALID_STATE:
            result << "Invalid state";
            break;
        case SUMMARY_NOT_SUPPORTED:
            result << "Not supported";
            break;
        case SUMMARY_INVALID_ARGUMENT:
            result << "Invalid argument";
            break;
        case SUMMARY_WRONG_ARGUMENT:
            result << "Wrong argument";
            break;
        case SUMMARY_CANCELED:
            result << "Canceled";
            break;
        case SUMMARY_STATUS_CHANGED:
            result << "Status changed";
            break;
        case SUMMARY_INTERNAL:
            result << "Internal";
            break;
        default:
            result << "Invalid summary";
            break;
    }

    result << " (0x" << std::hex << error.summary << ")" << "\n";

    result << "Description: ";
    switch(error.description) {
        case DESCRIPTION_SUCCESS:
            result << "Success";
            break;
        case DESCRIPTION_INVALID_MEMORY_PERMISSIONS:
            result << "Invalid memory permissions";
            break;
        case DESCRIPTION_INVALID_TICKET_VERSION:
            result << "Invalid ticket version";
            break;
        case DESCRIPTION_STRING_TOO_BIG:
            result << "String too big";
            break;
        case DESCRIPTION_ACCESS_DENIED:
            result << "Access denied";
            break;
        case DESCRIPTION_STRING_TOO_SMALL:
            result << "String too small";
            break;
        case DESCRIPTION_CAMERA_BUSY:
            result << "Camera busy";
            break;
        case DESCRIPTION_NOT_ENOUGH_MEMORY:
            result << "Not enough memory";
            break;
        case DESCRIPTION_SESSION_CLOSED_BY_REMOTE:
            result << "Session closed by remote";
            break;
        case DESCRIPTION_INVALID_NCCH:
            result << "Invalid NCCH";
            break;
        case DESCRIPTION_INVALID_TITLE_VERSION:
            result << "Invalid title version";
            break;
        case DESCRIPTION_DATABASE_DOES_NOT_EXIST:
            result << "Database does not exist";
            break;
        case DESCRIPTION_TRIED_TO_UNINSTALL_SYSTEM_APP:
            result << "Tried to uninstall system app";
            break;
        case DESCRIPTION_ARCHIVE_NOT_MOUNTED:
            result << "Archive not mounted";
            break;
        case DESCRIPTION_REQUEST_TIMED_OUT:
            result << "Request timed out";
            break;
        case DESCRIPTION_TITLE_NOT_FOUND:
            result << "Title not found";
            break;
        case DESCRIPTION_GAMECARD_NOT_INSERTED:
            result << "Gamecard not inserted";
            break;
        case DESCRIPTION_INVALID_FILE_OPEN_FLAGS:
            result << "Invalid file open flags";
            break;
        case DESCRIPTION_INVALID_CONFIGURATION:
            result << "Invalid configuration";
            break;
        case DESCRIPTION_NCCH_HASH_CHECK_FAILED:
            result << "NCCH hash check failed";
            break;
        case DESCRIPTION_AES_VERIFICATION_FAILED:
            result << "AES verification failed";
            break;
        case DESCRIPTION_SAVE_HASH_CHECK_FAILED:
            result << "Save hash check failed";
            break;
        case DESCRIPTION_COMMAND_PERMISSION_DENIED:
            result << "Command permission denied";
            break;
        case DESCRIPTION_INVALID_PATH:
            result << "Invalid path";
            break;
        case DESCRIPTION_INCORRECT_READ_SIZE:
            result << "Incorrect read size";
            break;
        case DESCRIPTION_INVALID_SECTION:
            result << "Invalid section";
            break;
        case DESCRIPTION_TOO_LARGE:
            result << "Too large";
            break;
        case DESCRIPTION_NOT_AUTHORIZED:
            result << "Not authorized";
            break;
        case DESCRIPTION_ALREADY_DONE:
            result << "Already done";
            break;
        case DESCRIPTION_INVALID_SIZE:
            result << "Invalid size";
            break;
        case DESCRIPTION_INVALID_ENUM_VALUE:
            result << "Invalid enum value";
            break;
        case DESCRIPTION_INVALID_COMBINATION:
            result << "Invalid combination";
            break;
        case DESCRIPTION_NO_DATA:
            result << "No data";
            break;
        case DESCRIPTION_BUSY:
            result << "Busy";
            break;
        case DESCRIPTION_MISALIGNED_ADDRESS:
            result << "Misaligned address";
            break;
        case DESCRIPTION_MISALIGNED_SIZE:
            result << "Misaligned size";
            break;
        case DESCRIPTION_OUT_OF_MEMORY:
            result << "Out of memory";
            break;
        case DESCRIPTION_NOT_IMPLEMENTED:
            result << "Not implemented";
            break;
        case DESCRIPTION_INVALID_ADDRESS:
            result << "Invalid address";
            break;
        case DESCRIPTION_INVALID_POINTER:
            result << "Invalid pointer";
            break;
        case DESCRIPTION_INVALID_HANDLE:
            result << "Invalid handle";
            break;
        case DESCRIPTION_NOT_INITIALIZED:
            result << "Not initialized";
            break;
        case DESCRIPTION_ALREADY_INITIALIZED:
            result << "Already initialized";
            break;
        case DESCRIPTION_NOT_FOUND:
            result << "Not found";
            break;
        case DESCRIPTION_CANCEL_REQUESTED:
            result << "Cancel requested";
            break;
        case DESCRIPTION_ALREADY_EXISTS:
            result << "Already exists";
            break;
        case DESCRIPTION_OUT_OF_RANGE:
            result << "Out of range";
            break;
        case DESCRIPTION_TIMEOUT:
            result << "Timeout";
            break;
        default:
            result << "Invalid description";
            break;
    }

    result << " (0x" << std::hex << error.description << ")" << "\n";

    return result.str();
}