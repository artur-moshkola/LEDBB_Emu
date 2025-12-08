#include <stdlib.h>
#include <string.h>
#include "hal.h"
#include "bb_led.h"
#include "ws2812-emu.h"

#define c0 {0, 0, 0}
#define cR {16, 0, 0}
#define cG {0, 16, 0}
#define cB {0, 0, 16}
#define cW {16, 16, 16}

#define cY {16, 16, 0}
#define cM {16, 0, 16}
#define cC {0, 16, 16}

#define cN {11, 0, 2}

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

uint8_t bb_led_is_handler(uint8_t (*handler)(uint8_t)) {
	return bb_led_frame_handler == handler;
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
		cR, cG, cB
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
		ws2812_set_led_safe(bb_led_no(BB_LED_MX, BB_LED_MY), (frame % (BB_LED_STB_FRAMES * 128) == 0) ? COLOR_STB : COLOR_OFF);
		return 1;
	}

	return 0;
}

void bb_led_set_bitmap(const ws2812_color (*map)[BB_LED_W]) {
	for (uint8_t x = 0; x < BB_LED_W; x++) {
		for (uint8_t y = 0; y < BB_LED_H; y++) {
			ws2812_set_led(bb_led_no(x, y), map[y][x]);
		}
	}
}

uint8_t bb_led_fhdl_xmass(uint8_t init) {
	static uint16_t frame;
	if (init) {
		frame = 0;
		srand(HAL_GetTick());
	}
	uint8_t changed;

	static const ws2812_color bg[BB_LED_H][BB_LED_W] = {
		{ c0, c0, c0, cG, c0, c0, c0, c0, c0, c0, c0, c0, cG, c0, c0, c0, c0, c0, c0, c0, c0, cG, c0, c0, c0, c0 },
		{ c0, c0, cG, cG, cG, c0, c0, c0, c0, c0, c0, cG, cG, cG, c0, c0, c0, c0, c0, c0, cG, cG, cG, c0, c0, c0 },
		{ c0, cG, cG, cG, cG, cG, c0, c0, c0, c0, cG, cG, cG, cG, cG, c0, c0, c0, c0, cG, cG, cG, cG, cG, c0, c0 },
		{ c0, c0, c0, cN, c0, c0, c0, c0, c0, c0, c0, c0, cN, c0, c0, c0, c0, c0, c0, c0, c0, cN, c0, c0, c0, c0 }
	};

	struct xy
	{
		uint8_t x;
		uint8_t y;
	};

	static const struct xy balls_places[] = {
		{ .x = 3, .y = 0 }, { .x = 12, .y = 0 }, { .x = 21, .y = 0 },
		{ .x = 2, .y = 1 }, { .x = 4, .y = 1 }, { .x = 11, .y = 1 }, { .x = 13, .y = 1 }, { .x = 20, .y = 1 }, { .x = 22, .y = 1 },
		{ .x = 1, .y = 2 }, { .x = 3, .y = 2 }, { .x = 5, .y = 2 }, { .x = 10, .y = 2 }, { .x = 12, .y = 2 }, { .x = 14, .y = 2 }, { .x = 19, .y = 2 }, { .x = 21, .y = 2 }, { .x = 23, .y = 2 }
	};

	static const ws2812_color ball_colors[] = {
		cR, cG, cY, cB
	};

	static uint8_t snow[BB_LED_H][BB_LED_W];

	if (init) {
		bb_led_set_bitmap(bg);
		changed = 1;

		memset(snow, 0, sizeof(snow));
	}

	if (frame % 5 == 0) {
		for (int i = 0; i < sizeof(balls_places) / sizeof(struct xy); i++) {
			struct xy place = balls_places[i];
			if (snow[place.y][place.x]) continue;
			int brnd = rand();
			ws2812_set_led(bb_led_no(place.x, place.y), ball_colors[brnd & 3]);
		}
	}
	
	if (frame % 20 == 0) {
		if (frame % 1000 == 0)
			memset(snow, 0, sizeof(snow));
		
		for (int y = BB_LED_H - 2; y >= 0; y--) {
			for (int x = 0; x < BB_LED_W; x++) {
				if (snow[y][x] && !snow[y + 1][x]) {
					snow[y + 1][x] = 1;
					snow[y][x] = 0;
				}
			}
		}
		
		int ns = rand() % BB_LED_W;
		if (!(snow[BB_LED_H - 1][ns] && snow[BB_LED_H - 2][ns]))
			snow[0][ns] = 1;

		for (int x = 0; x < BB_LED_W; x++) {
			for (int y = 0; y < BB_LED_H; y++) {
				int is_ball = 0;
				ws2812_color clr;
				for (int i = 0; i < sizeof(balls_places) / sizeof(struct xy); i++) {
					if (balls_places[i].x == x && balls_places[i].y == y) {
						clr = ball_colors[rand() & 3];
						is_ball = 1;
						break;
					}
				}
				if (snow[y][x]) {
					clr = (struct ws2812_color)cW;
				} else if (!is_ball) {
					clr = bg[y][x];
				}
				ws2812_set_led(bb_led_no(x, y), clr);
			}
		}

		changed = 1;
	}

	frame++;

	return changed;
}
