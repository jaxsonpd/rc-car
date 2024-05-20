/* File:   test_control.c
   Author: I Patel & D Hawes
   Date:   9 May 2024
   Descr:  Main file used for testing without radio
*/
#include <stdint.h>
#include <stdbool.h>

#include <pio.h>
#include "usb_serial.h" 
#include "pwm.h"
#include "delay.h"
#include "panic.h"
#include "pacer.h"
#include "ledtape.h"

#include "radio.h"
#include "motor_control.h"

#include "target.h"

#define DELAY_MS 10
#define NUM_LEDS 20

#define PACER_RATE 50
// #define RX_RATE 15
// #define TX_RATE 8

bool button_press = false;

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
    int left_duty;
    int right_duty;
    int dastardly;
    int parity;
    uint8_t hit_signal;
    // uint32_t tick_tx = 0;
    // uint32_t tick_rx = 0;
    char radio_message[33];
    bool listening = true;

    while (1)
    {   
        pacer_wait();

        delay_ms(DELAY_MS);
        char buf[256];
        if (fgets(buf, sizeof(buf), stdin)) {
            int left_motor_duty;
            int right_motor_duty;
            if (sscanf(buf, "%d %d", &left_motor_duty, &right_motor_duty) == 2){
                if (left_motor_duty >= 10) {
                    left_motor_duty = 10;
                } else if (left_motor_duty <= -10) {
                    left_motor_duty = -10;
                }
                if (right_motor_duty >= 10) {
                    right_motor_duty = 10;
                } else if (right_motor_duty <= -10) {
                    right_motor_duty = -10;
                }
                set_duty(left_motor_duty, right_motor_duty);
            }
        }

        ledtape_write (LEDTAPE_PIO, leds, NUM_LEDS*3);
        // tick_rx++;
        // tick_tx++;
        

        // if(tick_tx > (PACER_RATE/TX_RATE)) {
        //     pio_output_set(TX_LED, 0); // tells its in tranmitting mode
        //     pio_output_set (RX_LED, 1);
        //     int tx_status;

        //     //check for button press
        //     bool hit_detect = pio_input_get (BUMP_DETECT);

        //     tx_status = radio_tx(hit_detect);
        //     pio_output_set(TX_LED, 1);

        //     // if (tx_status == 1){
        //     //     // listening == true;   // once message has been sent, board is back in receiving mode
        //     //     pio_output_set(TX_LED, 1);
        //     // }
            
        //     tick_tx = 0;
        // } 

        // if (tick_rx > (PACER_RATE/RX_RATE)) {
        //     pio_output_set (RX_LED, 0);
        //     pio_output_set(TX_LED, 1);

        //     char buffer[32] = {0};

        //     radio_rx(buffer);

        //     printf("%s\n", buffer);
        //     printf("TX\n");

        //     sscanf(buffer, "%d,%d,%d,%d", &left_duty, 
        //     &right_duty,&dastardly,&parity);

        //     printf("Set: %3d,%3d,%2d,%1d\n", left_duty, right_duty, dastardly, 
        //     parity);

        //     set_duty(left_duty, right_duty);
            
        //     tick_rx = 0;
        //     // printf ("listening\n");
        }

        
    }
}
