#ifndef __CTRCOMMON_SCREEN_HPP__
#define __CTRCOMMON_SCREEN_HPP__

#include "ctrcommon/types.hpp"

#include <string>

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

#endif
