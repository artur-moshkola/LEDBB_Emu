#include "hal.h"
#include "bb_led.h"
#include "ws2812-emu.h"

uint32_t bb_led_tck;

uint8_t ihndlr;
uint8_t (*bb_led_frame_handler)(uint8_t);

void bb_led_init(void) {
	bb_led_tck = 0;
	emu_draw_init();
}

void bb_led_tick(void) {
	uint32_t tick = HAL_GetTick();
	if (tick - bb_led_tck < FRAME)
		return;
	bb_led_tck = tick;
	if (bb_led_frame_handler(ihndlr))
		emu_draw();
	ihndlr = 0;
}

void bb_led_set_handler(uint8_t (*handler)(uint8_t)) {
	if (bb_led_frame_handler == handler)
		return;
	ihndlr = 1;
	bb_led_frame_handler = handler;
}

uint32_t bb_led_no(uint32_t x, uint32_t y) {
	return (y | 1) * BB_LED_W + (((y & 1) - 1) ^ x);
}
/*
 *  0    x         25-x    25..0     00
 *  1    51-x      x+26    26..51    01
 *  2    x+52      77-x    77..52    10
 *  3    103-x     x+78    78..103   11
 *                        129..104  100
 */

uint8_t bb_led_fhdl_running(uint8_t init) {
	static uint16_t frame;
	if (init) frame = 0;
	uint8_t changed = 0;
	static const ws2812_color colors[] = {
		{ 16, 0, 0}, {0, 16, 0}, {0, 0, 16}
	};
	for (uint8_t i = 0; i < 9; i++) {
		for (uint8_t j = 0; j < 3; j++) {
			uint32_t x = i*3+((frame+j) % 3);
			if (x < BB_LED_W) {
				for (uint32_t y = 0; y < BB_LED_H; y++)
				   changed |= ws2812_set_led_step(bb_led_no(x, y), STEP, colors[j]);
			}
		}
	}

	if (!changed)
		frame++;

	return changed;
}

#define BB_LED_CHROMA_CMP(COLOR1, COLOR2) \
	(COLOR1.R == COLOR2.R && COLOR1.G == COLOR2.G && COLOR1.B == COLOR2.B)

uint8_t bb_led_fhdl_chromafill(uint8_t ihndlr) {
	static uint8_t frame;
	static ws2812_color prev;
	if (ihndlr) {
		frame = 0;
		prev.R = prev.G = prev.B = 0;
		bb_led_chromafill_color.R = bb_led_chromafill_color.G = bb_led_chromafill_color.B = BB_LED_CHROMA_MAX;
	}

	frame++;

	if (!BB_LED_CHROMA_CMP(prev, bb_led_chromafill_color)) {
		ws2812_set_all_leds(bb_led_chromafill_color);
		prev = bb_led_chromafill_color;
		return 1;
	}

	if (frame == BB_LED_CHROMA_FADE_FRAMES) {
		frame = 0;

		BB_LED_CHROMA_INC(R, BB_LED_CHROMA_FADE_STEP)
		BB_LED_CHROMA_INC(G, BB_LED_CHROMA_FADE_STEP)
		BB_LED_CHROMA_INC(B, BB_LED_CHROMA_FADE_STEP)
	}

	return 0;
}

uint8_t bb_led_fhdl_standby(uint8_t ihndlr) {
	static uint8_t faded;
	static uint8_t frame;
	if (ihndlr) {
		faded = 0;
	}

	uint8_t changed = 0;

	if (!faded) {
		for (uint16_t i = 0; i < WS2812_NUM_LEDS; i++) {
			changed |= ws2812_set_led_step(i, STEP, COLOR_OFF);
		}
		faded = !changed;
		return changed;
	}

	frame++;
	if (frame % BB_LED_STB_FRAMES == 0) {
		ws2812_set_led_safe(bb_led_no(BB_LED_MX, BB_LED_MY), (frame % (BB_LED_STB_FRAMES * 2) == 0) ? COLOR_STB : COLOR_OFF);
		return 1;
	}

	return 0;
}
