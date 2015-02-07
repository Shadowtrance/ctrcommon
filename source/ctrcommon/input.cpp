#include "ctrcommon/common.hpp"

#include <3ds.h>

void input_poll() {
    hidScanInput();
}

bool input_is_released(Button button) {
    return (hidKeysUp() & button) != 0;
}

bool input_is_pressed(Button button) {
    return (hidKeysDown() & button) != 0;
}

bool input_is_held(Button button) {
    return (hidKeysHeld() & button) != 0;
}

Touch input_get_touch() {
    touchPosition pos;
    hidTouchRead(&pos);
    return {pos.px, pos.py};
}