/**
 * @file racer.c
 * @authors Daniel Hawes (dha144@uclive.ac.nz) Isha Patel (Ipa46@uclive.ac.nz)
 * @date 2024-04-24
 * @brief This is the main file for the racer program 
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
    /* Configure STATE LED PIO as output. */
    pio_config_set (LED_STATUS_PIO, PIO_OUTPUT_HIGH);

    pacer_init(LED_FLASH_RATE);
    
    while (1) 
    {
        pacer_wait();

        pio_output_toggle (LED_STATUS_PIO);
    }
}
