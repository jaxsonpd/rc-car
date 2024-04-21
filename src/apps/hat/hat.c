/** 
 * @file hat.c
 * @author Jack Duignan (Jdu80@uclive.ac.nz)
 * @date 2024-04-22
 * @brief This is the main file for the hat program
 */


#include <stdint.h>
#include <stdbool.h>

#include <pio.h>

#include "pacer.h"

#include "target.h"

#define LED_FLASH_RATE 4

int
main (void)
{   
    // Setup
    pio_config_set(LED_STATUS_PIO, LED_ACTIVE);
    
    pacer_init(LED_FLASH_RATE);

    while (1) {
        pacer_wait();

        pio_output_toggle(LED_STATUS_PIO);
    }
}
