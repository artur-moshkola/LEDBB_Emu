#include <stdint.h>

#define BUTTONS 4

#define BTN_X 0
#define BTN_R 1
#define BTN_G 2
#define BTN_B 3

#define BUTTONS_KEYS 'x', 'r', 'g', 'b'

void bb_keys_init(void);
void bb_keys_tick(void);
void bb_keys_term(void);

uint8_t bb_keys_check(uint8_t btn);
uint8_t bb_keys_check_reset(uint8_t btn);