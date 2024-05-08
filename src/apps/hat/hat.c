/** 
 * @file hat.c
 * @author Jack Duignan (Jdu80@uclive.ac.nz)
 * @date 2024-04-22
 * @brief This is the main file for the hat program
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
#include "communications.h"
#include "target.h"

#define PACER_RATE 50
#define CONTROL_UPDATE_RATE 12
#define RADIO_SEND_RATE 6

control_data_t g_control_data; 
radio_packet_t g_radio_packet;

void setup (void) {
    usb_serial_stdio_init ();

    pio_config_set(LED_STATUS_PIO, PIO_OUTPUT_HIGH);
    pio_config_set(LED_ERROR_PIO, PIO_OUTPUT_HIGH);

    pio_output_set(LED_STATUS_PIO, LED_ACTIVE);

    pacer_init(PACER_RATE);

    if (radio_init()) {
        panic (LED_ERROR_PIO, 4);
    }

    if (control_init(ADXL345_ADDRESS, true)) {
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

            if ((get_result = control_get_data(&g_control_data)) != 0) {
                printf ("Acc Error\n");
            } else {
                ticks = 0;
            }
        } 

        if (ticks > (PACER_RATE / RADIO_SEND_RATE)) {
            g_radio_packet.left_duty = g_control_data.left_motor;
            g_radio_packet.right_duty = g_control_data.right_motor;
            g_radio_packet.parity = 1;
            if (serial_tx(&g_radio_packet)) {
                printf ("Tx Error\n");
            } else {
                ticks = 0;
            }
        }

    }
}
