/** 
 * @file led_tape.c
 * @author Jack Duignan (Jdu80@uclive.ac.nz)
 * @date 2024-05-20
 * @brief The implemenation for the led tap
 */


#include <stdint.h>
#include <stdbool.h>

#include "target.h"
#include "ledtape.h"
#include "led_tape.h"


int8_t led_tape_init(void) {


    return 0;
}


void led_tape_update(bool stopped) {
    static bool state = false;
    if (!stopped) {
        uint8_t leds[NUM_LEDS * 3];
        int i;

        for (i = 0; i < NUM_LEDS; i++)
        {
            // Set full green  GRB order
            leds[i * 3] = (143*state + 100*!state)/3;
            leds[i * 3 + 1] = (0*state + 0*!state)/3;
            leds[i * 3 + 2] = (30*state + 40*!state)/3;
        }

        ledtape_write (LEDTAPE_PIO, leds, NUM_LEDS * 3);

    } else {
        uint8_t leds[NUM_LEDS * 3];
        int i;

        for (i = 0; i < NUM_LEDS; i++)
        {
            // Set full green  GRB order
            leds[i * 3] = 0;
            leds[i * 3 + 1] = 255*state/3;
            leds[i * 3 + 2] = 0;
        }

        ledtape_write (LEDTAPE_PIO, leds, NUM_LEDS * 3);
    }

    state = !state;
}


void led_tape_off(void) {
        uint8_t leds[NUM_LEDS * 3];
        int i;

        for (i = 0; i < NUM_LEDS; i++)
        {
            // Set full green  GRB order
            leds[i * 3] = 0;
            leds[i * 3 + 1] = 0;
            leds[i * 3 + 2] = 0;
        }

        ledtape_write (LEDTAPE_PIO, leds, NUM_LEDS * 3);
}