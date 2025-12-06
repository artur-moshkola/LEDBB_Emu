#include <stdint.h>
#include "ws2812-emu.h"

#define FRAME 40
#define STEP 1
#define BB_LED_W 26
#define BB_LED_H 4

#define BB_LED_MX (BB_LED_W - 1)
#define BB_LED_MY (BB_LED_H - 1)

#define BB_LED_CHROMA_BTN_DEC_STEP 4
#define BB_LED_CHROMA_BTN_INC_STEP 2
#define BB_LED_CHROMA_FADE_STEP 1
#define BB_LED_CHROMA_FADE_FRAMES 32
#define BB_LED_CHROMA_MAX 16

#define BB_LED_CHROMA_INC(COLOR, STEP) \
	if ((bb_led_chromafill_color.COLOR > BB_LED_CHROMA_MAX) || ((BB_LED_CHROMA_MAX - bb_led_chromafill_color.COLOR) < STEP)) \
		bb_led_chromafill_color.COLOR = BB_LED_CHROMA_MAX; \
	else \
		bb_led_chromafill_color.COLOR += STEP;

#define BB_LED_CHROMA_DEC(COLOR, STEP) \
	if (bb_led_chromafill_color.COLOR > STEP) \
		bb_led_chromafill_color.COLOR -= STEP; \
	else \
		bb_led_chromafill_color.COLOR = 0;

extern ws2812_color bb_led_chromafill_color;

#define BB_LED_STB_FRAMES 16

static const ws2812_color COLOR_OFF = { 0, 0, 0 };
static const ws2812_color COLOR_STB = { 2, 2, 0 };

void bb_led_init(void);
void bb_led_tick(void);
void bb_led_set_handler(uint8_t (*handler)(uint8_t));

uint8_t bb_led_fhdl_running(uint8_t init);
uint8_t bb_led_fhdl_chromafill(uint8_t ihndlr);
uint8_t bb_led_fhdl_standby(uint8_t ihndlr);

uint8_t bb_led_fhdl_xmass(uint8_t ihndlr);
