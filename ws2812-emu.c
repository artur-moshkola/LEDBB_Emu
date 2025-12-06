#include "ws2812-emu.h"
#include <string.h>

uint8_t ws2812_leds[WS2812_NUM_LEDS][3];

void ws2812_init(void) {
    memset(ws2812_leds, 0, sizeof(ws2812_leds));
}

#define WS2812_SET_LED(LED_NO, COLOR) \
	ws2812_leds[LED_NO][WS2812_G] = COLOR.G; \
	ws2812_leds[LED_NO][WS2812_R] = COLOR.R; \
	ws2812_leds[LED_NO][WS2812_B] = COLOR.B;

void ws2812_set_led(uint32_t led_no, ws2812_color color) {
	WS2812_SET_LED(led_no, color);
}

void ws2812_set_led_safe(uint32_t led_no, ws2812_color color) {
	if (led_no >= WS2812_NUM_LEDS)
		return;

	WS2812_SET_LED(led_no, color)
}

#define WS2812_SET_LED_STEP_COLOR(LED_NO, COLOR_I, TARGET) \
	diff = TARGET - ws2812_leds[LED_NO][COLOR_I]; \
	changed |= (diff != 0); \
	if (diff > 0) \
		ws2812_leds[LED_NO][COLOR_I] += (diff < step) ? diff : step; \
	else \
		ws2812_leds[LED_NO][COLOR_I] += (diff > -step) ? diff : -step;

uint8_t ws2812_set_led_step(uint32_t led_no, uint8_t step, ws2812_color color) {
	uint8_t changed = 0;
	if (led_no >= WS2812_NUM_LEDS)
		return changed;

	int16_t diff;
	WS2812_SET_LED_STEP_COLOR(led_no, WS2812_G, color.G)
	WS2812_SET_LED_STEP_COLOR(led_no, WS2812_R, color.R)
	WS2812_SET_LED_STEP_COLOR(led_no, WS2812_B, color.B)

	return changed;
}

void ws2812_set_all_leds(ws2812_color color) {
    for (uint16_t i = 0; i < WS2812_NUM_LEDS; i++) {
    	WS2812_SET_LED(i, color)
    }
}
