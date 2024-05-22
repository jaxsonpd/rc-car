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
#include "battery.h"
#include "mcu_sleep.h"

// Game constants
#define STOP_TIME 5 //< The stop time in seconds
#define BATTERY_DROP_LIMIT 0 //< The number of times a battery drop is detected

// Pacer Constants in Hz
#define PACER_RATE 100
#define CONTROL_UPDATE_RATE 5
#define RADIO_SEND_RATE 30
#define RADIO_RECEIVE_RATE 50
#define BUZZER_UPDATE_RATE 4
#define LED_UPDATE_RATE 2
#define BATTERY_CHECK_RATE 1
#define SLEEP_CHECK_RATE 2

// Enables
bool g_radio_en = true;
bool g_control_en = true;
bool g_buzzer_en = true;
bool g_led_en = true;
bool g_battery_check_en = true;
bool g_sleep_en = true;

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
    g_radio_packet.dastardly = g_stopped;
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
    
    pio_config_set(BUTTON_PIO, PIO_PULLUP);

    if (radio_init()) {
        panic (LED_ERROR_PIO, 4);
    }

    if (!g_radio_en) {
        radio_power_down();
    }

    if (control_init(ADXL345_ADDRESS, false)) {
        panic (LED_ERROR_PIO, 12);
    }

    if (buzzer_init()) {
        panic (LED_ERROR_PIO, 6);
    }

    if (!g_buzzer_en) {
       buzzer_off();
    } 

    if (led_tape_init()) {
        panic (LED_ERROR_PIO, 8);
    }

    if (!g_led_en) {
        led_tape_off();
    }

    if (battery_sensor_init()) {
        panic (LED_ERROR_PIO, 10);
    }
}


void sleepify(void) {
    mcu_sleep_cfg_t sleep_cfg = {.mode=MCU_SLEEP_MODE_BACKUP};
    mcu_sleep_wakeup_cfg_t sleep_wakeup_cfg = {
        .pio = BUTTON_PIO,
        .active_high = false,
    };

    led_tape_off();
    radio_power_down();
    buzzer_off();
    pio_output_high(LED_STATUS_PIO);
    pio_output_high(LED_ERROR_PIO);

    delay_ms(3000);

    mcu_sleep_wakeup_set(&sleep_wakeup_cfg);
    mcu_sleep(&sleep_cfg);
}


int main (void) {
    uint32_t ticks_control = 0;
    uint32_t ticks_tx = 0;
    uint32_t ticks_rx = 0;
    uint32_t ticks_buzzer = 0;
    uint32_t ticks_led = 0;
    uint32_t ticks_battery = 0;
    uint32_t ticks_sleep = 0;

    uint32_t ticks_stopped = 0;

    uint32_t battery_drop_num = 0;

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
        ticks_battery++;
        ticks_sleep++;

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
                
                if (result == 0 && !g_stopped) {
                    g_bumper_hit = true;
                } else {
                    g_bumper_hit = false;
                }
            }

            ticks_rx = 0;
        } 

        // Buzzer sound update
        if (g_buzzer_en && (ticks_buzzer > (PACER_RATE / BUZZER_UPDATE_RATE))) {
            buzzer_update(g_stopped);
            ticks_buzzer = 0;
        }

        // Led strip update
        if (g_led_en && (ticks_led > (PACER_RATE / LED_UPDATE_RATE))) {
            led_tape_update(g_stopped);
            ticks_led = 0;
        }   

        // Battery voltage switch
        if (g_battery_check_en 
            && (ticks_battery > (PACER_RATE / BATTERY_CHECK_RATE))) {
            if (battery_millivolts() < 2553) {
                battery_drop_num++;
            } else {
                battery_drop_num = 0;
                pio_output_high(LED_ERROR_PIO);
            }

            if (battery_drop_num > BATTERY_DROP_LIMIT) {
                // radio_power_down();
                // led_tape_off();
                // buzzer_off();
                // pio_output_high(LED_STATUS_PIO);
                pio_output_toggle(LED_ERROR_PIO);   
            }

            ticks_battery = 0;
        }

        // Sleep checking
        if (g_sleep_en && ticks_sleep > (PACER_RATE/SLEEP_CHECK_RATE)) {
            
            if (pio_input_get(BUTTON_PIO) == 0) {
                sleepify();
            }
            ticks_sleep = 0;
        }
    }
}
