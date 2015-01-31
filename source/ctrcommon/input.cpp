#include "ctrcommon/common.hpp"

#include <3ds.h>

PAD_KEY buttonMap[13] = {
        KEY_A,
        KEY_B,
        KEY_X,
        KEY_Y,
        KEY_L,
        KEY_R,
        KEY_START,
        KEY_SELECT,
        KEY_UP,
        KEY_DOWN,
        KEY_LEFT,
        KEY_RIGHT,
        KEY_TOUCH
};

void input_poll() {
    hidScanInput();
}

bool input_is_released(Button button) {
    return (hidKeysUp() & buttonMap[button]) != 0;
}

bool input_is_pressed(Button button) {
    return (hidKeysDown() & buttonMap[button]) != 0;
}

bool input_is_held(Button button) {
    return (hidKeysHeld() & buttonMap[button]) != 0;
}

Touch input_get_touch() {
    touchPosition pos;
    hidTouchRead(&pos);
	return {pos.px, pos.py};
}