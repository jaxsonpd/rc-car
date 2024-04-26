/** 
 * @file CTR_Test.c
 * @author Jack Duignan (Jdu80@uclive.ac.nz)
 * @date 2024-04-25
 * @brief This file is used to test the control module
 * 
 * Build Options:
 * DRIVERS = pacer usb_serial adxl345 panic
 * SRC = control_test.c control.c circular_buffer.c
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include <pio.h>

#include "pacer.h"
#include "usb_serial.h"
#include "panic.h"
#include "delay.h"

#include "control.h"
#include "target.h"

#define PACER_RATE 50
#define CONTROL_UPDATE_RATE 6


control_data_t g_control_data; 


void setup (void) {
    usb_serial_stdio_init ();

    pio_config_set(LED_STATUS_PIO, PIO_OUTPUT_HIGH);
    pio_config_set(LED_ERROR_PIO, PIO_OUTPUT_HIGH);

    pio_output_set(LED_STATUS_PIO, LED_ACTIVE);

    pacer_init(PACER_RATE);

    if (control_init(ADXL345_ADDRESS, false)) {
        panic (LED_ERROR_PIO, 2);
    }
}


int main (void) {
    int32_t ticks = 0;

    setup();

    while (1) {
        pacer_wait();
        ticks++;

        control_update();

        if (ticks > (PACER_RATE / CONTROL_UPDATE_RATE)) {
            int8_t get_result;

            if ((get_result = control_get_data(&g_control_data)) == 0) {
                //printf ("x: %5d  y: %5d  z: %5d, %1d\n", g_controlData.raw_x, 
                //g_controlData.raw_y, g_controlData.raw_z, updateResult);
                printf ("%5d, %5d,  %5d\n", g_control_data.raw_x, 
                g_control_data.raw_y, g_control_data.raw_z);
            } else {
                printf ("Acc Error\n");
            }
        } 
    }
}
