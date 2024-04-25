/** 
 * @file CTR_Test.c
 * @author Jack Duignan (Jdu80@uclive.ac.nz)
 * @date 2024-04-25
 * @brief This file is used to test the control module
 * 
 * Build Options:
 * DRIVERS = pacer usb_serial adxl345 panic
 * SRC = hat.c CTR_control.c 
 */

#include <stdint.h>
#include <stdbool.h>

#include <pio.h>

#include "pacer.h"
#include "usb_serial.h"
#include "panic.h"

#include "CTR_control.h"
#include "target.h"

#define PACER_RATE 50
#define CONTROL_UPDATE_RATE 25


controlData_t g_controlData; 


void setup (void) {
    pio_config_set(LED_STATUS_PIO, LED_ACTIVE);
    pio_config_set(LED_ERROR_PIO, !LED_ACTIVE);
    
    pacer_init(PACER_RATE);

    if (CTR_init(ADXL345_ADDRESS)) {
        panic (LED_ERROR_PIO, 2);
    }

    usb_serial_stdio_init ();
}


int main (void) {
    int32_t ticks = 0;

    setup();
    printf("Setup Complete");

    while (1) {
        pacer_wait();
        ticks++;

        if (ticks > (PACER_RATE / CONTROL_UPDATE_RATE)) {
            int8_t updateResult;
            if ((updateResult = CTR_update(&g_controlData)) == 0) {
                printf ("x: %5d  y: %5d  z: %5d, %1d\n", g_controlData.raw_x, 
                g_controlData.raw_y, g_controlData.raw_z, updateResult);
            } else {
                printf ("Acc Error\n");
            }
        } 
    }
}
