/* File:   battery.c
   Author: I Patel, D Hawes
   Date:   May 2024
   Descr:  Functions for different LED tape patterns
*/


#include <stdint.h>
#include <stdbool.h>

#include "target.h"
#include "ledtape.h"
#include "led_tape.h"

#define NUM_LEDS 20

// sets the Led tape to general light blue colour of Finn McMissile
void led_tape_driving(void) {
    uint8_t leds[NUM_LEDS * 3];
    int i;

    for (i = 0; i < NUM_LEDS; i++)
    {
        // Set full green  GRB order
        leds[i * 3] = 50/3;      //G
        leds[i * 3 + 1] = 40/3;    //R
        leds[i * 3 + 2] = 255/2;    //B
    }
    ledtape_write (LEDTAPE_PIO, leds, NUM_LEDS * 3);
}

// sets the Led tape to rotate through red
void led_tape_bump(void) {
    static bool state = false;
    uint8_t leds[NUM_LEDS * 3];
    int i;

    for (i = 0; i < NUM_LEDS; i++)
    {
        // Set full green  GRB order
        leds[i * 3] = 0;
        leds[i * 3 + 1] = 150;
        leds[i * 3 + 2] = 0;
    }

    ledtape_write (LEDTAPE_PIO, leds, NUM_LEDS * 3);
    state = !state;

}

// turns all led off
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