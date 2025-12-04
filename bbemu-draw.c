#include <stdio.h>
#include "bb_led.h"
#include "bb_blink.h"

uint8_t bbemu_draw_brightness = 4;

uint32_t bb_led_no(uint32_t x, uint32_t y);

uint8_t emu_draw_amplify(uint8_t value) {
    if (value >= (0xff >> bbemu_draw_brightness))
        return 0xff;
    return value << bbemu_draw_brightness;
}

ws2812_color emu_draw_get_color(uint32_t led_no) {
    ws2812_color color;
    color.R = emu_draw_amplify(ws2812_leds[led_no][WS2812_R]);
    color.G = emu_draw_amplify(ws2812_leds[led_no][WS2812_G]);
    color.B = emu_draw_amplify(ws2812_leds[led_no][WS2812_B]);
    return color;
}

void emu_draw() {
	printf("\x1b[%dF", BB_LED_H * 2 + 2);
    for (uint32_t y = 0; y < BB_LED_H; y++) {
        for (uint32_t x = 0; x < BB_LED_W; x++) {
            ws2812_color color = emu_draw_get_color(bb_led_no(x, y));
            printf("\x1b[48;2;%d;%d;%dm  \x1b[0m ", color.R, color.G, color.B);
        }
        printf("\n\n");
    }
    printf("Brightness: x%d   \n\n", 1 << bbemu_draw_brightness);
}

void emu_draw_init() {
    for (uint32_t y = 0; y < BB_LED_H; y++)
        printf("\n\n");
    printf("\n");
}