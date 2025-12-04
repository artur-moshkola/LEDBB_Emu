#include <stdint.h>
#include "bbemu-draw.h"

#ifndef WS2812
#define WS2812

#define WS2812_NUM_LEDS 104

#define WS2812_G 0
#define WS2812_R 1
#define WS2812_B 2

typedef struct ws2812_color {
	uint8_t R;
	uint8_t G;
	uint8_t B;
} ws2812_color;

extern uint8_t ws2812_leds[WS2812_NUM_LEDS][3];

void ws2812_init(void);
void ws2812_send_spi(void);
void ws2812_send_spi_DMA(void);
void ws2812_set_led(uint32_t led_no, ws2812_color color);
void ws2812_set_all_leds(ws2812_color color);
void ws2812_set_led_safe(uint32_t led_no, ws2812_color color);
uint8_t ws2812_set_led_step(uint32_t led_no, uint8_t step, ws2812_color color);

#endif
