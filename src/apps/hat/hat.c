/** 
 * @file hat.c
 * @author Jack Duignan (Jdu80@uclive.ac.nz)
 * @date 2024-04-22
 * @brief This is the main file for the hat program
 */

/**
 * Overview:
 * 
 * This program does 3 things:
 * Transmits the accelerometer readings as duty cycles at a rate of 
 * RADIO_SEND_RATE unless the bumper is hit when it send 0s
 * If the bumper is hit it activates the led strip
 * 
 * It plays a song on the buzzer
 * 
 * Receives only when data is available at a rate of RADIO_RX_RATE there is 
 * no waiting after a successful rx so care should be taken to ensure that tx 
 * from the other device is not to large.
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
#include "buzzer.h"
#include "led_tape.h"

// Game constants
#define STOP_TIME 3 //< The stop time in seconds

// Pacer Constants in Hz
#define PACER_RATE 100
#define CONTROL_UPDATE_RATE 5
#define RADIO_SEND_RATE 5
#define RADIO_RECEIVE_RATE 50
#define BUZZER_UPDATE_RATE 3
#define LED_UPDATE_RATE 1

// Enables
bool g_radio_en = false;
bool g_control_en = false;
bool g_buzzer_en = true;
bool g_led_en = true;

control_data_t g_control_data; 
radio_packet_t g_radio_packet;

// Bumper control
bool g_bumper_hit = false;
bool g_stopped = false;

/** 
 * @brief Handle the radio tx functionality
 * 
 */
void radio_tx_handler(void) {
    int8_t get_result;
    if ((get_result = control_get_data(&g_control_data)) != 0) {
        printf ("Acc Error\n");
    }

    if (g_stopped) {
        g_control_data.left_motor = 0;
        g_control_data.right_motor = 0;
    }

    g_radio_packet.left_duty = g_control_data.left_motor;
    g_radio_packet.right_duty = g_control_data.right_motor;
    g_radio_packet.dastardly = 0;
    g_radio_packet.parity = 1;

    if (radio_tx(&g_radio_packet, true)) {
        // printf ("Tx Error\n");
    }
}


void setup (void) {
    usb_serial_stdio_init ();

    pio_config_set(LED_STATUS_PIO, PIO_OUTPUT_HIGH);
    pio_config_set(LED_ERROR_PIO, PIO_OUTPUT_HIGH);
    pio_config_set(RADIO_POWER_ENABLE_PIO, PIO_OUTPUT_HIGH);

    pio_output_set(LED_STATUS_PIO, LED_ACTIVE);

    if (radio_init()) {
        panic (LED_ERROR_PIO, 4);
    }

    if (!g_radio_en) {
        radio_power_down();
        pio_output_set(RADIO_POWER_ENABLE_PIO, 0);
    }

    if (control_init(ADXL345_ADDRESS, true)) {
        panic (LED_ERROR_PIO, 2);
    }

    if (buzzer_init()) {
        panic (LED_ERROR_PIO, 6);
    }

    if (led_tape_init()) {
        panic (LED_ERROR_PIO, 8);
    }
}


int main (void) {
    uint32_t ticks_control = 0;
    uint32_t ticks_tx = 0;
    uint32_t ticks_rx = 0;
    uint32_t ticks_buzzer = 0;
    uint32_t ticks_led = 0;

    uint32_t ticks_stopped = 0;

    setup();

    pacer_init(PACER_RATE);

    while (1) {
        pacer_wait();
        ticks_control++;
        ticks_rx++;
        ticks_tx++;
        ticks_buzzer++;
        ticks_led++;
        ticks_stopped++;

        // Stopped from bumper control
        if (g_bumper_hit && !g_stopped 
            && (ticks_stopped > (STOP_TIME+2) * PACER_RATE)) 
        {
            g_stopped = true;
            ticks_stopped = 0;
            g_bumper_hit = false;
        } else if (g_stopped && (ticks_stopped > STOP_TIME*PACER_RATE)) {
            g_stopped = false;
        } else if (ticks_stopped > (UINT32_MAX - 100)) { // Control over flow
            ticks_stopped = 0;
        }

    	// Control Update
        if (g_control_en && (ticks_control > (PACER_RATE / CONTROL_UPDATE_RATE))) {
            control_update();
            ticks_control = 0;
        } 

        // Radio tx
        if (g_radio_en && (ticks_tx > (PACER_RATE / RADIO_SEND_RATE))) {
            radio_tx_handler();

            radio_listen(); // Set back to read mode
            ticks_tx = 0;
        }

        // Radio Rx
        if (g_radio_en && (ticks_rx > (PACER_RATE / RADIO_RECEIVE_RATE))) {
            if (radio_rx_data_ready()) { // only read if you have too
                int8_t result = radio_get_bumper();

                if (result != -1) {
                    printf("                             Rx: %d\n", result);   
                } 
                
                if (result == 1) {
                    g_bumper_hit = true;
                }
            }

            ticks_rx = 0;
        } 

        // Buzzer sound update
        if (g_buzzer_en && (ticks_buzzer > (PACER_RATE / BUZZER_UPDATE_RATE))) {
            buzzer_update();
            ticks_buzzer = 0;
        }

        // Led strip update
        if (g_led_en && (ticks_led > (PACER_RATE / LED_UPDATE_RATE))) {
            led_tape_update(g_stopped);
            ticks_led = 0;
        }   
    }
}
