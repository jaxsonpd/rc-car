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

#include "radio.h"

#include "target.h"

#define PWM1_PIO PA25_PIO //AIN1
#define PWM2_PIO PB14_PIO //BIN1
#define PWM3_PIO PB1_PIO //AIN2
#define PWM4_PIO PB0_PIO //BIN2

#define N_FAULT PA28_PIO // nFault pin
#define N_SLEEP PA29_PIO // nSleep pin

#define BUMP_DETECT PA27_PIO // bumper pin

#define DELAY_MS 10


// If you are using PWM to drive a motor you will need
// to choose a lower frequency!
#define PWM_FREQ_HZ 2000
#define START_DUTY_CYCLE 0

uint32_t duty_cycle_right = 50;
uint32_t duty_cycle_left = 50;

int duty_cycle_right_forwards = 0;
int duty_cycle_right_backwards = 0;
int duty_cycle_left_forwards = 0;
int duty_cycle_left_backwards = 0;

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

    // creates variables for setting duty
    int8_t left_duty;
    int8_t right_duty;
    uint8_t dastardly;
    uint8_t parity;
    char radio_message;
    bool listening = true;

    while (1)
    {
        // dont include if just testing receive
        // found rf_tester file that has cleaner changing between modes
        if (BUMP_DETECT == 0){  // is active low
            pio_output_set(TX_LED, 0); // tells its in tranmitting mode
            pio_output_set (RX_LED, 1);
            listening = false;
        } else {
            pio_output_set (RX_LED, 0);
            pio_output_set(TX_LED, 1);
            listening = true;
            printf ("listening\n");
        }

        // checks status and calls tx or rx functions accordingly
        if (listening == false) {
            int tx_status;
            tx_status = radio_tx();
            if (tx_status == 1){
                listening == true;   // once message has been sent, board is back in receiving mode
                pio_output_set(TX_LED, 1);
            }
            // just use this bottom part if not testing with tx
        } else {
            radio_message = radio_rx();
            // will need to check this works the way i think it does
            // read based on what is actually being sent to the car ( compared to hats comms code)
            sscanf(radio_message, "%hhd,%hhd,%hhd", &left_duty, &right_duty, &parity);  // gives warning: passing argument 1 of 'sscanf' makes pointer from integer without a cast [-Wint-conversion]
            // dastardly not being sent by hat currently

        }

        set_duty(pwm1, pwm2, pwm3, pwm4, left_duty, right_duty);
    }

    return 0;
}
