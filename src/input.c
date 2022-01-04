// input.c

#include <gbdk/platform.h>

#include "input.h"

UINT8 keys = 0x00;
UINT8 previous_keys = 0x00;
UINT8 key_repeat_count = 0x00;

void wait_no_keys_lowcpu(void) {

    while (ANY_KEY_PRESSED) {
        wait_vbl_done(); // yield CPU
        UPDATE_KEYS();
    };
}