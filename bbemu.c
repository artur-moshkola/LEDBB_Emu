#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include "hal.h"
#include "ws2812-emu.h"
#include "bb_blink.h"
#include "bb_led.h"
#include "bb_keys.h"

ws2812_color bb_led_chromafill_color;

volatile int keepRunning = 1;

void intHandler(int dummy) {
    keepRunning = 0;
}

int main() {    
    signal(SIGINT, intHandler);

    ws2812_init();
    bb_blink_init();
    bb_keys_init();
    bb_led_init();
    bb_led_set_handler(bb_led_fhdl_running);

    uint8_t failed = 0;
    uint32_t tick;

    uint8_t bi = 0;
    uint32_t btick;

    uint32_t iatick = HAL_GetTick();

    while (keepRunning)
    {
        tick = HAL_GetTick();

        bb_blink_tick();
        if (failed) continue;
        uint32_t blink_tick = HAL_GetTick();

        bb_led_tick();
        uint32_t led_tick = HAL_GetTick();

        bb_keys_tick();
        uint32_t keys_tick = HAL_GetTick();

        if (bb_keys_check_reset(BTN_X)) {
            bb_blink_pattern = 0b101010;
            iatick = btick = tick; bi = 1;
            bb_led_set_handler(bb_led_fhdl_running);
        }
        if (bb_keys_check_reset(BTN_R)) {
            bb_blink_pattern = 0b10101010;
            iatick = btick = tick; bi = 1;
            bb_led_set_handler(bb_led_fhdl_chromafill);
            BB_LED_CHROMA_INC(R, BB_LED_CHROMA_BTN_INC_STEP)
            BB_LED_CHROMA_DEC(G, BB_LED_CHROMA_BTN_DEC_STEP)
            BB_LED_CHROMA_DEC(B, BB_LED_CHROMA_BTN_DEC_STEP)
        }
        if (bb_keys_check_reset(BTN_G)) {
            bb_blink_pattern = 0b1010101010;
            iatick = btick = tick; bi = 1;
            bb_led_set_handler(bb_led_fhdl_chromafill);
            BB_LED_CHROMA_DEC(R, BB_LED_CHROMA_BTN_DEC_STEP)
            BB_LED_CHROMA_INC(G, BB_LED_CHROMA_BTN_INC_STEP)
            BB_LED_CHROMA_DEC(B, BB_LED_CHROMA_BTN_DEC_STEP)
        }
        if (bb_keys_check_reset(BTN_B)) {
            bb_blink_pattern = 0b101010101010;
            iatick = btick = tick; bi = 1;
            bb_led_set_handler(bb_led_fhdl_chromafill);
            BB_LED_CHROMA_DEC(R, BB_LED_CHROMA_BTN_DEC_STEP)
            BB_LED_CHROMA_DEC(G, BB_LED_CHROMA_BTN_DEC_STEP)
            BB_LED_CHROMA_INC(B, BB_LED_CHROMA_BTN_INC_STEP)
        }
        if (bi && (HAL_GetTick() - btick > 5000)) {
            bi = 0;
            bb_blink_init();
        }
        if (tick - iatick > 90000) {
            bb_led_set_handler(bb_led_fhdl_standby);
        }

        uint32_t btn_tick = HAL_GetTick();

        HAL_Sleep();
    }

    bb_keys_term();

    printf("Bye\n");
    
    return 0;
}