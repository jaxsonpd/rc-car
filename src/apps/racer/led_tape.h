/* File:   battery.c
   Author: I Patel, D Hawes
   Date:   May 2024
   Descr:  Functions for different LED tape patterns
*/


#ifndef LED_TAPE_H
#define LED_TAPE_H


#include <stdint.h>
#include <stdbool.h>

#define NUM_LEDS 20


// sets the Led tape to general light blue colour of Finn McMissile
void led_tape_driving(void);

// sets the Led tape to rotate through red
void led_tape_bump(void);

// turns all led off
void led_tape_off(void);


#endif // LED_TAPE_H