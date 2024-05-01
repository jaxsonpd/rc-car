/** 
 * @file buzzer.c
 * @author Ethan Wildash-Chan (ewi69@uclive.ac.nz)
 * @date 2024-04-30
 * @brief Uses the piezo buzzer to play a melody
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include <pio.h>

void buzzer_init (void) {
    pio_config_set(BUZZER_PIO PIO_OUTPUT_HIGH);
}