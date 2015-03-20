#ifndef __CTRCOMMON_SCREEN_HPP__
#define __CTRCOMMON_SCREEN_HPP__

#include "ctrcommon/types.hpp"

#include <string>

typedef enum {
    TOP_SCREEN,
    BOTTOM_SCREEN
} Screen;

bool screenBeginDraw(Screen screen);
bool screenEndDraw();
void screenSwapBuffersQuick();
void screenSwapBuffers();
u16 screenGetWidth();
u16 screenGetHeight();
bool screenReadPixels(u8* dest, int srcX, int srcY, int dstX, int dstY, u16 width, u16 height);
bool screenTakeScreenshot();
bool screenDraw(int x, int y, u8 r, u8 g, u8 b);
bool screenFill(int x, int y, u16 width, u16 height, u8 r, u8 g, u8 b);
bool screenCopy(int x, int y, u16 width, u16 height, u8* pixels);
bool screenClear(u8 r, u8 g, u8 b);
void screenClearBuffers(Screen screen, u8 r, u8 g, u8 b);
void screenClearAll(u8 r, u8 g, u8 b);
u16 screenGetStrWidth(const std::string str);
u16 screenGetStrHeight(const std::string str);
bool screenDrawChar(char c, int x, int y, u8 r, u8 g, u8 b);
bool screenDrawString(const std::string str, int x, int y, u8 r, u8 g, u8 b);

#endif
