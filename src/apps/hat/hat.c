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
#define CONTROL_UPDATE_RATE 25
#define RADIO_SEND_RATE 10
#define RADIO_RECIVE_RATE 25

control_data_t g_control_data; 
radio_packet_t g_radio_packet;

void setup (void) {
    usb_serial_stdio_init ();

    pio_config_set(LED_STATUS_PIO, PIO_OUTPUT_HIGH);
    pio_config_set(LED_ERROR_PIO, PIO_OUTPUT_HIGH);

    pio_output_set(LED_STATUS_PIO, LED_ACTIVE);

    if (radio_init()) {
        panic (LED_ERROR_PIO, 4);
    }

    if (control_init(ADXL345_ADDRESS, true)) {
        panic (LED_ERROR_PIO, 2);
    }
}


int main (void) {
    uint32_t ticks_control = 0;
    uint32_t ticks_tx = 0;
    uint32_t ticks_rx = 0;

    setup();

    pacer_init(PACER_RATE);

    while (1) {
        pacer_wait();
        ticks_control++;
        ticks_rx++;
        ticks_tx++;

        control_update();

        if (ticks_control > (PACER_RATE / CONTROL_UPDATE_RATE)) {
            int8_t get_result;
            if ((get_result = control_get_data(&g_control_data)) != 0) {
                printf ("Acc Error\n");
            }

            ticks_control = 0;
        } 

        if (ticks_tx > (PACER_RATE / RADIO_SEND_RATE)) {
            g_radio_packet.left_duty = g_control_data.left_motor;
            g_radio_packet.right_duty = g_control_data.right_motor;
            g_radio_packet.dastardly = 0;
            g_radio_packet.parity = 1;

            if (radio_tx(&g_radio_packet, true)) {
                // printf ("Tx Error\n");
            }

            ticks_tx = 0;
        }

        if (ticks_rx > (PACER_RATE / RADIO_RECIVE_RATE)) {
            int8_t result = radio_get_bumper();

            if (result == -1) {
                // printf("                             Bumper Error\n");
            } else {
                printf("                             Rx: %d\n", result);
                pio_output_set(LED_STATUS_PIO, !result);
            }


            ticks_rx = 0;
        } 

    }
}
