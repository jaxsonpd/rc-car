/* File:   radio_drive.c
   Author: I Patel & D Hawes
   Date:   9 May 2024
   Descr:  combines hbridge file and calls radio functions 
*/
#include <stdint.h>
#include <stdbool.h>

#include <pio.h>
#include "usb_serial.h" 
#include "pwm.h"
#include "delay.h"
#include "panic.h"
#include "pacer.h"

#include "radio.h"
#include "motor_control.h"

#include "target.h"

#define DELAY_MS 10

#define PACER_RATE 100
#define RX_RATE 50
#define TX_RATE 10

#define RAMP_STEP 1
#define RAMP_DELAY 10

bool button_press = false;

// void ramp_duty_cycle(int *current_left_duty, int target_left_duty, int *current_right_duty, int target_right_duty) {
//     while (*current_left_duty != target_left_duty || *current_right_duty != target_right_duty) {
//         printf("RAMP\n");
//         // printf("Left Motor: %d, Right Motor: %d", current_left_duty, current_right_duty);
//         if (*current_left_duty < target_left_duty) {
//             *current_left_duty += RAMP_STEP;
//             if (*current_left_duty > target_left_duty) {
//                 *current_left_duty = target_left_duty;
//             }
//         } else if (*current_left_duty > target_left_duty) {
//             *current_left_duty -= RAMP_STEP;
//             if (*current_left_duty < target_left_duty) {
//                 *current_left_duty = target_left_duty;
//             }
//         }

//         if (*current_right_duty < target_right_duty) {
//             *current_right_duty += RAMP_STEP;
//             if (*current_right_duty > target_right_duty) {
//                 *current_right_duty = target_right_duty;
//             }
//         } else if (*current_right_duty > target_right_duty) {
//             *current_right_duty -= RAMP_STEP;
//             if (*current_right_duty < target_right_duty) {
//                 *current_right_duty = target_right_duty;
//             }
//         }

//         set_duty(*current_left_duty, *current_right_duty); // Update both motors
//         delay_ms(RAMP_DELAY);
//     }
//     printf("Ramp done\n");
// }

// void ramp_duty_cycle(int *current_left_duty, int target_left_duty, int *current_right_duty, int target_right_duty) 
// {
//     printf("MOTOR SET %d, %d\n", target_left_duty, target_right_duty);
//     set_duty(target_left_duty, target_right_duty);
// }

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

    bool asked = false;

    pio_config_set (LED_STATUS_PIO, PIO_OUTPUT_HIGH);
    

    int i = 0;
    if(usb_serial_stdio_init() < 0) 
        panic(LED_ERROR_PIO, 3);

    pio_config_set (LED_STATUS_PIO, PIO_OUTPUT_HIGH);
    
    pacer_init(PACER_RATE);

    // all new below //

    radio_init();   // init radio
    pio_config_set(BUMP_DETECT, PIO_INPUT); // sets bumper pin as input
    pio_config_set(BUMP_DETECT, PIO_PULLUP);

    // creates variables for setting duty
    int prev_left_duty = 0;
    int prev_right_duty = 0;
    int left_motor_duty = 0;
    int right_motor_duty = 0;
    int dastardly;
    int parity;
    uint8_t hit_signal;
    uint32_t tick_tx = 0;
    uint32_t tick_rx = 0;
    char radio_message[33];
    bool listening = true;

    while (1)
    {   
        pacer_wait();
        tick_rx++;
        tick_tx++;

        // set_duty(0, 0);
        

        if(tick_tx > (PACER_RATE/TX_RATE)) {
            pio_output_set(TX_LED, 0); // tells its in tranmitting mode
            pio_output_set (RX_LED, 1);
            int tx_status;

            //check for button press
            bool hit_detect = pio_input_get (BUMP_DETECT);

            tx_status = radio_tx(hit_detect);
            pio_output_set(TX_LED, 1);
            
            radio_listen();
            tick_tx = 0;
        } 

        if (tick_rx > (PACER_RATE/RX_RATE)) {
            if (radio_rx_data_ready()) {
                printf("Listening\n");
                pio_output_set (RX_LED, 0);
                pio_output_set(TX_LED, 1);

                char buffer[32] = {0};

                radio_rx(buffer);

                printf("%s\n", buffer);
                printf("TX\n");

                sscanf(buffer, "%d,%d,%d,%d", &left_motor_duty, 
                &right_motor_duty,&dastardly,&parity);

                printf("Set: %3d,%3d,%2d,%1d\n", left_motor_duty, right_motor_duty, dastardly, parity);

                if (left_motor_duty >= 50) {
                    left_motor_duty = 50;
                } else if (left_motor_duty <= -50) {
                    left_motor_duty = -50;
                }
                if (right_motor_duty >= 50) {
                    right_motor_duty = 50;
                } else if (right_motor_duty <= -50) {
                    right_motor_duty = -50;
                }
                if (left_motor_duty<=5 && left_motor_duty>=-5) {
                    left_motor_duty = 0;
                } else if (right_motor_duty<=5 && left_motor_duty>=-5) {
                    right_motor_duty = 0;
                }
                // ramp_duty_cycle(&prev_left_duty, left_motor_duty, &prev_right_duty, right_motor_duty);
                // prev_right_duty=right_motor_duty;
                // prev_left_duty=left_motor_duty;
                set_duty(left_motor_duty, right_motor_duty);
            }
            tick_rx = 0;
            // printf ("listening\n");
        }

        
    }
}
