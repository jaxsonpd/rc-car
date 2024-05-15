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

#include "target.h"

#define PWM1_PIO PA25_PIO //AIN1
#define PWM2_PIO PB14_PIO //BIN1
#define PWM3_PIO PB1_PIO //AIN2
#define PWM4_PIO PB0_PIO //BIN2

#define N_FAULT PA28_PIO // nFault pin
#define N_SLEEP PA29_PIO // nSleep pin

#define DELAY_MS 10

#define PACER_RATE 50
#define RX_RATE 15
#define TX_RATE 8

// If you are using PWM to drive a motor you will need
// to choose a lower frequency!
#define PWM_FREQ_HZ 2000
#define START_DUTY_CYCLE 0

uint32_t duty_cycle_right = 0;
uint32_t duty_cycle_left = 0;

int duty_cycle_right_forwards = 0;
int duty_cycle_right_backwards = 0;
int duty_cycle_left_forwards = 0;
int duty_cycle_left_backwards = 0;

bool button_press = false;

//Configure all 4 PWM pins

static const pwm_cfg_t pwm1_cfg =
{
    .pio = PWM1_PIO,
    .period = PWM_PERIOD_DIVISOR (PWM_FREQ_HZ),
    .duty = PWM_DUTY_DIVISOR (PWM_FREQ_HZ, START_DUTY_CYCLE),
    .align = PWM_ALIGN_LEFT,
    .polarity = PWM_POLARITY_HIGH,
    .stop_state = PIO_OUTPUT_LOW,
};


static const pwm_cfg_t pwm2_cfg =
{
    .pio = PWM2_PIO,
    .period = PWM_PERIOD_DIVISOR (PWM_FREQ_HZ),
    .duty = PWM_DUTY_DIVISOR (PWM_FREQ_HZ, START_DUTY_CYCLE),
    .align = PWM_ALIGN_LEFT,
    .polarity = PWM_POLARITY_HIGH,
    .stop_state = PIO_OUTPUT_LOW,
};


static const pwm_cfg_t pwm3_cfg =
{
    .pio = PWM3_PIO,
    .period = PWM_PERIOD_DIVISOR (PWM_FREQ_HZ),
    .duty = PWM_DUTY_DIVISOR (PWM_FREQ_HZ, START_DUTY_CYCLE),
    .align = PWM_ALIGN_LEFT,
    .polarity = PWM_POLARITY_HIGH,
    .stop_state = PIO_OUTPUT_LOW,
};


static const pwm_cfg_t pwm4_cfg =
{
    .pio = PWM4_PIO,
    .period = PWM_PERIOD_DIVISOR (PWM_FREQ_HZ),
    .duty = PWM_DUTY_DIVISOR (PWM_FREQ_HZ, START_DUTY_CYCLE),
    .align = PWM_ALIGN_LEFT,
    .polarity = PWM_POLARITY_HIGH,
    .stop_state = PIO_OUTPUT_LOW,
};


// adapts hbridge code to set the pwm duty for each motor 
void set_duty(pwm_t pwm1, pwm_t pwm2, pwm_t pwm3, pwm_t pwm4, int duty_cycle_left, int duty_cycle_right) {
    if (duty_cycle_right > 0) {
        duty_cycle_right_forwards = duty_cycle_right;
        duty_cycle_right_backwards = 0;
    } else {
        duty_cycle_right_backwards = -duty_cycle_right;
        duty_cycle_right_forwards = 0;
    }

    if (duty_cycle_left > 0) {
        duty_cycle_left_forwards = duty_cycle_left;
        duty_cycle_left_backwards = 0;
    } else {
        duty_cycle_left_backwards = -duty_cycle_left;
        duty_cycle_left_forwards = 0;
    }
    
    // writes duty to the pins on the h bridge ( unchanged by isha )
    pwm_duty_set(pwm1, PWM_DUTY_DIVISOR(PWM_FREQ_HZ, duty_cycle_left_forwards));
    pwm_duty_set(pwm2, PWM_DUTY_DIVISOR(PWM_FREQ_HZ, duty_cycle_right_forwards));
    pwm_duty_set(pwm3, PWM_DUTY_DIVISOR(PWM_FREQ_HZ, duty_cycle_left_backwards));
    pwm_duty_set(pwm4, PWM_DUTY_DIVISOR(PWM_FREQ_HZ, duty_cycle_right_backwards));
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

    pwm_t pwm1;
    pwm_t pwm2;
    pwm_t pwm3;
    pwm_t pwm4;

    bool asked = false;

    pio_config_set (LED_STATUS_PIO, PIO_OUTPUT_HIGH);
    pio_config_set(N_FAULT, PIO_INPUT);
    pio_config_set(N_FAULT, PIO_OUTPUT_HIGH);
    pio_config_set(N_SLEEP, PIO_OUTPUT_HIGH);
    pio_output_high(N_SLEEP);

    int i = 0;
    if(usb_serial_stdio_init() < 0) 
        panic(LED_ERROR_PIO, 3);

    pio_config_set (LED_STATUS_PIO, PIO_OUTPUT_HIGH);
    
    pacer_init(PACER_RATE);

    pwm1 = pwm_init (&pwm1_cfg);
    if (! pwm1)
        panic (LED_ERROR_PIO, 1);

    pwm2 = pwm_init (&pwm2_cfg);
    if (! pwm2)
        panic (LED_ERROR_PIO, 1);

    pwm3 = pwm_init (&pwm3_cfg);
    if (! pwm3)
        panic (LED_ERROR_PIO, 1);

    pwm4 = pwm_init (&pwm4_cfg);
    if (! pwm4)
        panic (LED_ERROR_PIO, 1);

    pwm_start(pwm1);
    pwm_start(pwm2);
    pwm_start(pwm3);
    pwm_start(pwm4);

    pwm_duty_set(pwm1, PWM_DUTY_DIVISOR(PWM_FREQ_HZ, 0));
    pwm_duty_set(pwm2, PWM_DUTY_DIVISOR(PWM_FREQ_HZ, 0));
    pwm_duty_set(pwm3, PWM_DUTY_DIVISOR(PWM_FREQ_HZ, 0));
    pwm_duty_set(pwm4, PWM_DUTY_DIVISOR(PWM_FREQ_HZ, 0));

    // all new below //

    radio_init();   // init radio
    pio_config_set(BUMP_DETECT, PIO_INPUT); // sets bumper pin as input
    pio_config_set(BUMP_DETECT, PIO_PULLUP);

    // creates variables for setting duty
    int left_duty;
    int right_duty;
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
        

        if(tick_tx > (PACER_RATE/TX_RATE)) {
            pio_output_set(TX_LED, 0); // tells its in tranmitting mode
            pio_output_set (RX_LED, 1);
            int tx_status;

            //check for button press
            bool hit_detect = pio_input_get (BUMP_DETECT);

            tx_status = radio_tx(hit_detect);
            pio_output_set(TX_LED, 1);

            // if (tx_status == 1){
            //     // listening == true;   // once message has been sent, board is back in receiving mode
            //     pio_output_set(TX_LED, 1);
            // }
            
            tick_tx = 0;
        } 

        if (tick_rx > (PACER_RATE/RX_RATE)) {
            pio_output_set (RX_LED, 0);
            pio_output_set(TX_LED, 1);

            char buffer[32] = {0};

            radio_rx(buffer);

            printf("%s\n", buffer);
            printf("TX\n");

            sscanf(buffer, "%d,%d,%d,%d", &left_duty, 
            &right_duty,&dastardly,&parity);

            printf("Set: %3d,%3d,%2d,%1d\n", left_duty, right_duty, dastardly, 
            parity);

            set_duty(pwm1, pwm2, pwm3, pwm4, left_duty, right_duty);
            
            tick_rx = 0;
            // printf ("listening\n");
        }

        
    }
}
