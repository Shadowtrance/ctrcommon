#ifndef __COMMON_H__
#define __COMMON_H__

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

bool platform_init();
void platform_cleanup();
bool platform_is_running();
u64 platform_get_time();
void platform_delay(int ms);
void platform_printf(const char* format, ...);
bool platform_is_io_waiting();

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

const std::string app_get_platform_name(AppPlatform platform);
const std::string app_get_category_name(AppCategory category);
std::vector<App> app_list(MediaType mediaType);
int app_install_file(MediaType mediaType, const std::string path, std::function<bool(int progress)> onProgress);
int app_install_socket(MediaType mediaType, int socket, u64 size, std::function<bool(int progress)> onProgress);
int app_delete(App app);
int app_launch(App app);

u64 fs_get_free_space(MediaType mediaType);

typedef enum {
    BUTTON_A,
    BUTTON_B,
    BUTTON_X,
    BUTTON_Y,
    BUTTON_L,
    BUTTON_R,
    BUTTON_START,
    BUTTON_SELECT,
    BUTTON_UP,
    BUTTON_DOWN,
    BUTTON_LEFT,
    BUTTON_RIGHT,
    BUTTON_TOUCH
} Button;

typedef struct {
    int x;
    int y;
} Touch;

void input_poll();
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
bool screen_fill(int x, int y, u16 width, u16 height, u8 r, u8 g, u8 b);
bool screen_clear(u8 r, u8 g, u8 b);
u16 screen_get_str_width(const std::string str);
u16 screen_get_str_height(const std::string str);
bool screen_draw_string(const std::string str, int x, int y, u8 r, u8 g, u8 b);

u64 htonll(u64 value);
u64 ntohll(u64 value);
u32 socket_get_host_ip();
int socket_listen(u16 port);
int socket_accept(int fd);
int socket_connect(const std::string ipAddress, u16 port);
int socket_read(int fd, void* buffer, u32 bufferSize);
int socket_write(int fd, void* buffer, u32 bufferSize);
void socket_close(int fd);

typedef struct {
	std::string id;
	std::string name;
	std::vector<std::string> details;
} SelectableElement;

typedef struct {
	int socket;
	u64 fileSize;
} RemoteFile;

bool ui_select(SelectableElement* selected, std::vector<SelectableElement> elements, std::function<bool(std::vector<SelectableElement>& currElements, bool& elementsDirty)> onLoop, std::function<bool(SelectableElement select)> onSelect);
bool ui_select_file(std::string* selectedFile, const std::string rootDirectory, std::vector<std::string> extensions, std::function<bool()> onLoop);
bool ui_select_app(App* selectedApp, MediaType mediaType, std::function<bool()> onLoop);
void ui_display_message(const std::string message);
bool ui_prompt(const std::string message, bool question);
void ui_display_progress(const std::string operation, const std::string details, bool quickSwap, int progress);
RemoteFile ui_accept_remote_file();

#endif