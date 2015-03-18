#ifndef __CTRCOMMON_COMMON_HPP__
#define __CTRCOMMON_COMMON_HPP__

#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>

#include <vector>
#include <string>
#include <functional>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

typedef enum {
    MODULE_COMMON = 0,
    MODULE_KERNEL = 1,
    MODULE_UTIL = 2,
    MODULE_FILE_SERVER = 3,
    MODULE_LOADER_SERVER = 4,
    MODULE_TCB = 5,
    MODULE_OS = 6,
    MODULE_DBG = 7,
    MODULE_DMNT = 8,
    MODULE_PDN = 9,
    MODULE_GX = 10,
    MODULE_I2C = 11,
    MODULE_GPIO = 12,
    MODULE_DD = 13,
    MODULE_CODEC = 14,
    MODULE_SPI = 15,
    MODULE_PXI = 16,
    MODULE_FS = 17,
    MODULE_DI = 18,
    MODULE_HID = 19,
    MODULE_CAM = 20,
    MODULE_PI = 21,
    MODULE_PM = 22,
    MODULE_PM_LOW = 23,
    MODULE_FSI = 24,
    MODULE_SRV = 25,
    MODULE_NDM = 26,
    MODULE_NWM = 27,
    MODULE_SOC = 28,
    MODULE_LDR = 29,
    MODULE_ACC = 30,
    MODULE_ROMFS = 31,
    MODULE_AM = 32,
    MODULE_HIO = 33,
    MODULE_UPDATER = 34,
    MODULE_MIC = 35,
    MODULE_FND = 36,
    MODULE_MP = 37,
    MODULE_MPWL = 38,
    MODULE_AC = 39,
    MODULE_HTTP = 40,
    MODULE_DSP = 41,
    MODULE_SND = 42,
    MODULE_DLP = 43,
    MODULE_HIO_LOW = 44,
    MODULE_CSND = 45,
    MODULE_SSL = 46,
    MODULE_AM_LOW = 47,
    MODULE_NEX = 48,
    MODULE_FRIENDS = 49,
    MODULE_RDT = 50,
    MODULE_APPLET = 51,
    MODULE_NIM = 52,
    MODULE_PTM = 53,
    MODULE_MIDI = 54,
    MODULE_MC = 55,
    MODULE_SWC = 56,
    MODULE_FATFS = 57,
    MODULE_NGC = 58,
    MODULE_CARD = 59,
    MODULE_CARDNOR = 60,
    MODULE_SDMC = 61,
    MODULE_BOSS = 62,
    MODULE_DBM = 63,
    MODULE_CONFIG = 64,
    MODULE_PS = 65,
    MODULE_CEC = 66,
    MODULE_IR = 67,
    MODULE_UDS = 68,
    MODULE_PL = 69,
    MODULE_CUP = 70,
    MODULE_GYROSCOPE = 71,
    MODULE_MCU = 72,
    MODULE_NS = 73,
    MODULE_NEWS = 74,
    MODULE_RO = 75,
    MODULE_GD = 76,
    MODULE_CARD_SPI = 77,
    MODULE_EC = 78,
    MODULE_WEB_BROWSER = 80,
    MODULE_TEST = 81,
    MODULE_ENC = 82,
    MODULE_PIA = 83,
    MODULE_MVD = 92,
    MODULE_QTM = 96,
    MODULE_APPLICATION = 254,
    MODULE_INVALID = 255
} ErrorModule;

typedef enum {
    LEVEL_SUCCESS = 0,
    LEVEL_INFO = 1,
    LEVEL_STATUS = 25,
    LEVEL_TEMPORARY = 26,
    LEVEL_PERMANENT = 27,
    LEVEL_USAGE = 28,
    LEVEL_REINITIALIZE = 29,
    LEVEL_RESET = 30,
    LEVEL_FATAL = 31
} ErrorLevel;

typedef enum {
    SUMMARY_SUCCESS = 0,
    SUMMARY_NOTHING_HAPPENED = 1,
    SUMMARY_WOULD_BLOCK = 2,
    SUMMARY_OUT_OF_RESOURCE = 3,
    SUMMARY_NOT_FOUND = 4,
    SUMMARY_INVALID_STATE = 5,
    SUMMARY_NOT_SUPPORTED = 6,
    SUMMARY_INVALID_ARGUMENT = 7,
    SUMMARY_WRONG_ARGUMENT = 8,
    SUMMARY_CANCELED = 9,
    SUMMARY_STATUS_CHANGED = 10,
    SUMMARY_INTERNAL = 11,
    SUMMARY_INVALID = 63
} ErrorSummary;

typedef enum {
    DESCRIPTION_SUCCESS = 0,
    DESCRIPTION_INVALID_MEMORY_PERMISSIONS = 2,
    DESCRIPTION_INVALID_TICKET_VERSION = 4,
    DESCRIPTION_STRING_TOO_BIG = 5,
    DESCRIPTION_ACCESS_DENIED = 6,
    DESCRIPTION_STRING_TOO_SMALL = 7,
    DESCRIPTION_CAMERA_BUSY = 8,
    DESCRIPTION_NOT_ENOUGH_MEMORY = 10,
    DESCRIPTION_SESSION_CLOSED_BY_REMOTE = 26,
    DESCRIPTION_INVALID_NCCH = 37,
    DESCRIPTION_INVALID_TITLE_VERSION = 39,
    DESCRIPTION_DATABASE_DOES_NOT_EXIST = 43,
    DESCRIPTION_TRIED_TO_UNINSTALL_SYSTEM_APP = 44,
    DESCRIPTION_ARCHIVE_NOT_MOUNTED = 101,
    DESCRIPTION_REQUEST_TIMED_OUT = 105,
    DESCRIPTION_TITLE_NOT_FOUND = 120,
    DESCRIPTION_GAMECARD_NOT_INSERTED = 141,
    DESCRIPTION_INVALID_FILE_OPEN_FLAGS = 230,
    DESCRIPTION_INVALID_CONFIGURATION = 271,
    DESCRIPTION_NCCH_HASH_CHECK_FAILED = 391,
    DESCRIPTION_AES_VERIFICATION_FAILED = 392,
    DESCRIPTION_SAVE_HASH_CHECK_FAILED = 395,
    DESCRIPTION_COMMAND_PERMISSION_DENIED = 630,
    DESCRIPTION_INVALID_PATH = 702,
    DESCRIPTION_INCORRECT_READ_SIZE = 761,
    DESCRIPTION_INVALID_SECTION = 1000,
    DESCRIPTION_TOO_LARGE = 1001,
    DESCRIPTION_NOT_AUTHORIZED = 1002,
    DESCRIPTION_ALREADY_DONE = 1003,
    DESCRIPTION_INVALID_SIZE = 1004,
    DESCRIPTION_INVALID_ENUM_VALUE = 1005,
    DESCRIPTION_INVALID_COMBINATION = 1006,
    DESCRIPTION_NO_DATA = 1007,
    DESCRIPTION_BUSY = 1008,
    DESCRIPTION_MISALIGNED_ADDRESS = 1009,
    DESCRIPTION_MISALIGNED_SIZE = 1010,
    DESCRIPTION_OUT_OF_MEMORY = 1011,
    DESCRIPTION_NOT_IMPLEMENTED = 1012,
    DESCRIPTION_INVALID_ADDRESS = 1013,
    DESCRIPTION_INVALID_POINTER = 1014,
    DESCRIPTION_INVALID_HANDLE = 1015,
    DESCRIPTION_NOT_INITIALIZED = 1016,
    DESCRIPTION_ALREADY_INITIALIZED = 1017,
    DESCRIPTION_NOT_FOUND = 1018,
    DESCRIPTION_CANCEL_REQUESTED = 1019,
    DESCRIPTION_ALREADY_EXISTS = 1020,
    DESCRIPTION_OUT_OF_RANGE = 1021,
    DESCRIPTION_TIMEOUT = 1022,
    DESCRIPTION_INVALID = 1023
} ErrorDescription;

typedef struct {
    u32 raw;
    ErrorModule module;
    ErrorLevel level;
    ErrorSummary summary;
    ErrorDescription description;
} Error;

bool platform_init();
void platform_cleanup();
bool platform_is_running();
bool platform_is_ninjhax();
u32 platform_get_device_id();
u64 platform_get_time();
void platform_delay(int ms);
void platform_printf(const char* format, ...);
Error platform_get_error();
void platform_set_error(Error error);
std::string platform_get_error_string(Error error);

typedef enum {
    NAND,
    SD
} MediaType;

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

u64 fs_get_free_space(MediaType mediaType);
bool fs_exists(const std::string path);
bool fs_delete(const std::string path);

#ifndef BIT
#define BIT(n) (1U << (n))
#define BIT_SELF_DEFINED
#endif
typedef enum {
    BUTTON_A = BIT(0),
    BUTTON_B = BIT(1),
    BUTTON_SELECT = BIT(2),
    BUTTON_START = BIT(3),
    BUTTON_DRIGHT = BIT(4),
    BUTTON_DLEFT = BIT(5),
    BUTTON_DUP = BIT(6),
    BUTTON_DDOWN = BIT(7),
    BUTTON_R = BIT(8),
    BUTTON_L = BIT(9),
    BUTTON_X = BIT(10),
    BUTTON_Y = BIT(11),
    BUTTON_ZL = BIT(14),
    BUTTON_ZR = BIT(15),
    BUTTON_TOUCH = BIT(20),
    BUTTON_CSTICK_RIGHT = BIT(24),
    BUTTON_CSTICK_LEFT = BIT(25),
    BUTTON_CSTICK_UP = BIT(26),
    BUTTON_CSTICK_DOWN = BIT(27),
    BUTTON_CPAD_RIGHT = BIT(28),
    BUTTON_CPAD_LEFT = BIT(29),
    BUTTON_CPAD_UP = BIT(30),
    BUTTON_CPAD_DOWN = BIT(31),
    BUTTON_UP = BUTTON_DUP | BUTTON_CPAD_UP,
    BUTTON_DOWN = BUTTON_DDOWN | BUTTON_CPAD_DOWN,
    BUTTON_LEFT = BUTTON_DLEFT | BUTTON_CPAD_LEFT,
    BUTTON_RIGHT = BUTTON_DRIGHT | BUTTON_CPAD_RIGHT,
} Button;
#ifdef BIT_SELF_DEFINED
#undef BIT
#undef BIT_SELF_DEFINED
#endif

typedef struct {
    int x;
    int y;
} Touch;

const std::string input_get_button_name(Button button);
void input_poll();
bool input_is_any_pressed();
Button input_get_any_pressed();
bool input_is_released(Button button);
bool input_is_pressed(Button button);
bool input_is_held(Button button);
Touch input_get_touch();

typedef struct {
    u8 r;
    u8 g;
    u8 b;
} Color;

typedef enum {
    TOP_SCREEN,
    BOTTOM_SCREEN
} Screen;

bool screen_begin_draw(Screen screen);
bool screen_end_draw();
void screen_swap_buffers_quick();
void screen_swap_buffers();
u16 screen_get_width();
u16 screen_get_height();
bool screen_read_pixels(u8* dest, int srcX, int srcY, int dstX, int dstY, u16 width, u16 height);
bool screen_take_screenshot();
bool screen_draw(int x, int y, u8 r, u8 g, u8 b);
bool screen_draw_packed(int x, int y, u32 color);
bool screen_fill(int x, int y, u16 width, u16 height, u8 r, u8 g, u8 b);
bool screen_copy(int x, int y, u16 width, u16 height, u8* pixels);
bool screen_clear(u8 r, u8 g, u8 b);
void screen_clear_buffers(Screen screen, u8 r, u8 g, u8 b);
void screen_clear_all(u8 r, u8 g, u8 b);
u16 screen_get_str_width(const std::string str);
u16 screen_get_str_height(const std::string str);
bool screen_draw_char(char c, int x, int y, u8 r, u8 g, u8 b);
bool screen_draw_string(const std::string str, int x, int y, u8 r, u8 g, u8 b);

u64 htonll(u64 value);
u64 ntohll(u64 value);
u32 socket_get_host_ip();
int socket_listen(u16 port);
FILE* socket_accept(int listeningSocket);
FILE* socket_connect(const std::string ipAddress, u16 port);

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

typedef struct {
	std::string id;
	std::string name;
	std::vector<std::string> details;
} SelectableElement;

typedef struct {
	FILE* fd;
	u64 fileSize;
} RemoteFile;

bool ui_select(SelectableElement* selected, std::vector<SelectableElement> elements, std::function<bool(std::vector<SelectableElement>& currElements, bool& elementsDirty, bool &resetCursorIfDirty)> onLoop, std::function<bool(SelectableElement select)> onSelect, bool useTopScreen = true, bool alphabetize = true);
bool ui_select_file(std::string* selectedFile, const std::string rootDirectory, std::vector<std::string> extensions, std::function<bool(bool inRoot)> onLoop, bool useTopScreen = true);
bool ui_select_app(App* selectedApp, MediaType mediaType, std::function<bool()> onLoop, bool useTopScreen = true);
void ui_display_message(Screen screen, const std::string message);
bool ui_prompt(Screen screen, const std::string message, bool question);
void ui_display_progress(Screen screen, const std::string operation, const std::string details, bool quickSwap, int progress);
RemoteFile ui_accept_remote_file(Screen screen);

#endif