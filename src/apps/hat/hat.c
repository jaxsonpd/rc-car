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

#include "usb_serial.h"
#include "control.h"
#include "target.h"

#define LED_FLASH_RATE 4


control_data_t g_control_data;


int main (void) {   
    // Setup
    pio_config_set(LED_STATUS_PIO, LED_ACTIVE);
    control_init(ADXL345_ADDRESS, false);
    usb_serial_stdio_init();
    
    pacer_init(LED_FLASH_RATE);

    while (1) {
        pacer_wait();
        printf("On\n");    
        control_update();
        control_get_data(&g_control_data);
        printf ("%5d, %5d,  %5d\n", g_control_data.raw_x, 
                g_control_data.raw_y, g_control_data.raw_z);
        pio_output_toggle(LED_STATUS_PIO);
    }
}

