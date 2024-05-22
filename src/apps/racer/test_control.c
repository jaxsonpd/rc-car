/* File:   test_control.c
   Author: I Patel & D Hawes
   Date:   9 May 2024
   Descr:  Main file used for testing without radio
*/
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>


#include <pio.h>
#include "usb_serial.h" 
#include "nrf24.h"
#include "pwm.h"
#include "delay.h"
#include "panic.h"
#include "pacer.h"
#include "led_tape.h"
#include "adc.h"
#include "battery.h"
#include "mcu_sleep.h"

#include "radio.h"
#include "motor_control.h"

#include "target.h"

#define DELAY_MS 10
#define NUM_LEDS 20

#define BATTERY_RATE 1
#define SLEEP_CHECK_RATE 1
#define BUMP_OFF_RATE 500

#define RAMP_STEP 1
#define RAMP_DELAY 10

#define PACER_RATE 50

// #define RX_RATE 15
// #define TX_RATE 8

bool button_press = false;

void sleepify(void) {
    mcu_sleep_cfg_t sleep_cfg = {.mode=MCU_SLEEP_MODE_BACKUP};
    mcu_sleep_wakeup_cfg_t sleep_wakeup_cfg = {
        .pio = BUTTON_PIO,
        .active_high = false,
    };

    led_tape_off();
    radio_power_down();
    // motor_power_down();
    pio_output_high(LED_STATUS_PIO);
    pio_output_high(LED_ERROR_PIO);

    delay_ms(3000);

    mcu_sleep_wakeup_set(&sleep_wakeup_cfg);
    mcu_sleep(&sleep_cfg);
}



void ramp_duty_cycle(int *current_left_duty, int target_left_duty, int *current_right_duty, int target_right_duty) {
    while (*current_left_duty != target_left_duty || *current_right_duty != target_right_duty) {
        // printf("Left Motor: %d, Right Motor: %d", current_left_duty, current_right_duty);
        if (*current_left_duty < target_left_duty) {
            *current_left_duty += RAMP_STEP;
            if (*current_left_duty > target_left_duty) {
                *current_left_duty = target_left_duty;
            }
        } else if (*current_left_duty > target_left_duty) {
            *current_left_duty -= RAMP_STEP;
            if (*current_left_duty < target_left_duty) {
                *current_left_duty = target_left_duty;
            }
        }

        if (*current_right_duty < target_right_duty) {
            *current_right_duty += RAMP_STEP;
            if (*current_right_duty > target_right_duty) {
                *current_right_duty = target_right_duty;
            }
        } else if (*current_right_duty > target_right_duty) {
            *current_right_duty -= RAMP_STEP;
            if (*current_right_duty < target_right_duty) {
                *current_right_duty = target_right_duty;
            }
        }

        set_duty(*current_left_duty, *current_right_duty); // Update both motors
        delay_ms(RAMP_DELAY);
    }
}

void bump_detect(int prev_left_duty, int prev_right_duty) 
{
    printf("BUMP\n");
    set_duty(0, 0);
    delay_ms(5000);
    // set_duty(-prev_left_duty, -prev_right_duty);
    // printf("Left: %d, Right: %d",-prev_left_duty, -prev_right_duty);
    // delay_ms(2000);
}

/***
 * adapts h bridge main to include init radio and ready to recieve radio
 * unsure if radio_rx needs to be in the while loop or seperate
 * will also write the values to the serial to allow for debugging
 * radio_tx is written, but will need to be triggered by the bumper
 * * the bumper acts as a switch to move the radio from rx to tx
 * * could set up bumper as an interrupt??
 * if it shits itself, could be bc of attempting serial and radio???
 ***/

int main (void)
{
    // init pwm //
    motor_init();
    led_tape_driving();

    battery_sensor_init();

    bool asked = false;

    pio_config_set (LED_STATUS_PIO, PIO_OUTPUT_HIGH);
    pio_config_set (LED_ERROR_PIO, PIO_OUTPUT_HIGH);

    pio_config_set(BUTTON_PIO, PIO_PULLUP);
    

    int i = 0;
    if(usb_serial_stdio_init() < 0) 
        panic(LED_ERROR_PIO, 3);

    
    pacer_init(PACER_RATE);

    uint8_t leds[NUM_LEDS * 3];
    for (i = 0; i < NUM_LEDS; i++)
    {
        // Set full green  GRB order
        leds[i * 3] = 255;
        leds[i * 3 + 1] = 0;
        leds[i * 3 + 2] = 0;
    }

    // all new below //

    // radio_init();   // init radio
    pio_config_set(BUMP_DETECT, PIO_INPUT); // sets bumper pin as input
    pio_config_set(BUMP_DETECT, PIO_PULLUP);

    // creates variables for setting duty
    int prev_left_duty = 0;
    int prev_right_duty = 0;
    int dastardly;
    int parity;
    uint8_t hit_signal;
    uint32_t tick_battery = 0;
    uint32_t tick_sleep = 0;
    bool hit_detect = false;
    // uint32_t tick_tx = 0;
    // uint32_t tick_rx = 0;
    char radio_message[33];
    bool listening = true;

    while (1)
    {   
        pacer_wait();
        tick_battery++;
        tick_sleep++;

        // delay_ms(DELAY_MS);


        char buf[256];
        if (fgets(buf, sizeof(buf), stdin)) {
            int left_motor_duty;
            int right_motor_duty;
            if (sscanf(buf, "%d %d", &left_motor_duty, &right_motor_duty) == 2){
                // set_duty(0,0);
                printf("Left Motor: %d Right Motor %d\n", left_motor_duty, right_motor_duty);
                printf("Channel: %d\n", radio_channel_number_get());
                printf("BATTERY: %d\n",  battery_millivolts());
                if (left_motor_duty >= 80) {
                    left_motor_duty = 80;
                } else if (left_motor_duty <= -80) {
                    left_motor_duty = -80;
                }
                if (right_motor_duty >= 80) {
                    right_motor_duty = 80;
                } else if (right_motor_duty <= -80) {
                    right_motor_duty = -80;
                }

                if (left_motor_duty == 0 && right_motor_duty == 0) {
                    led_tape_bump();
                } else {
                    led_tape_driving();
                }

                ramp_duty_cycle(&prev_left_duty, left_motor_duty, &prev_right_duty, right_motor_duty);
                prev_right_duty=right_motor_duty;
                prev_left_duty=left_motor_duty;
                // set_duty(0, 0);
            }
        }

        if(!pio_input_get (BUMP_DETECT)) {
            bump_detect(prev_left_duty, prev_right_duty);
        }   

        //Check Battery Value
        if (tick_battery>(PACER_RATE/BATTERY_RATE)) {
            if (battery_millivolts() < 2553)
            {
                pio_output_toggle(LED_ERROR_PIO);
                pio_output_high(LED_STATUS_PIO);
            }  else {
                pio_output_low(LED_STATUS_PIO);
                pio_output_high(LED_ERROR_PIO);
            }
            tick_battery=0;
        }

        if (tick_sleep > (PACER_RATE/SLEEP_CHECK_RATE)) {
            if (pio_input_get(BUTTON_PIO) == 0) {
                sleepify();
            }
            tick_sleep = 0;
        }
    }
}
