#include <stdio.h>
#include "bb_blink.h"

uint32_t bb_blink_pattern;

void bb_blink_init(void) {
	bb_blink_pattern = 0b1010;
}

const char *const ed_01[] = { "\x1b[34m0\x1b[0m", "\x1b[31m1\x1b[0m" };

void bb_blink_tick(void) {
	printf("\x1b[FBlinking pattern: ");
    uint32_t bbp = bb_blink_pattern;
    for (int i = 0; i < 32; i++) {
        fputs(ed_01[bbp & 1], stdout);
        bbp >>= 1;
    }
    printf("\n");
}
