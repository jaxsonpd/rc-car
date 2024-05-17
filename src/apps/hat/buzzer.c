/** 
 * @file buzzer.c
 * @author Ethan Wildash-Chan (ewi69@uclive.ac.nz)
 * @date 2024-04-30
 * @brief Uses the piezo buzzer to play a melody
 * https://github.com/robsoncouto/arduino-songs/blob/master/miichannel/miichannel.ino - website for example songs
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include <pio.h>

uint8_t buzzer_init (void) {
    pio_config_set (BUZZER_PIO, PIO_OUTPUT_HIGH);
    return 0;
}


void buzzer_update(void) {
    pio_output_toggle(BUZZER_PIO);
}

