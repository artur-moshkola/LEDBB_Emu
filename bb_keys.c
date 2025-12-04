#include <unistd.h>
#include <termios.h>
#include "bb_keys.h"

uint8_t bb_keys_flags[BUTTONS];

extern uint8_t bbemu_draw_brightness;

struct termios oldt;

const char keys[] = { BUTTONS_KEYS };

void bb_keys_init(void) {
	for (uint8_t i = 0; i < BUTTONS; i++) {
		bb_keys_flags[i] = 0;
	}

	struct termios newt;
	
	tcgetattr(STDIN_FILENO, &oldt);
	newt = oldt;
	newt.c_lflag &= ~(ICANON | ECHO); // Disable canonical mode and echoing
	newt.c_cc[VMIN] = 0; // Return immediately if no characters available
	newt.c_cc[VTIME] = 0; // No timeout for read
	tcsetattr(STDIN_FILENO, TCSANOW, &newt);
}

extern volatile int keepRunning;

void bb_keys_tick(void) {
	uint8_t buf[128];
	int8_t rd = read(STDIN_FILENO, buf, sizeof(buf));
	for (int c = 0; c < rd; c++) {
		uint8_t kpressed = buf[c];

		if (kpressed == 'q') keepRunning = 0;

		if (kpressed == '=' && bbemu_draw_brightness < 8) bbemu_draw_brightness++;
		if (kpressed == '-' && bbemu_draw_brightness > 0) bbemu_draw_brightness--;

		for (int i = 0; i < BUTTONS; i++) {
			if (keys[i] == kpressed)
				bb_keys_flags[i] = 1;
		}
	}
}

void bb_keys_term(void) {
	tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
}

uint8_t bb_keys_check(uint8_t btn) {
	return bb_keys_flags[btn];
}

uint8_t bb_keys_check_reset(uint8_t btn) {
	uint8_t val = bb_keys_flags[btn];
	bb_keys_flags[btn] = 0;
	return val;
}
