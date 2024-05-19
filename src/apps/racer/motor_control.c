/* File:   motor_control.c
   Author: I Patel & D Hawes
   Date:   9 May 2024
   Descr:  modulisation of pwm control
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

#define PWM1_PIO PA25_PIO //AIN1
#define PWM2_PIO PB14_PIO //BIN1
#define PWM3_PIO PB1_PIO //AIN2
#define PWM4_PIO PB0_PIO //BIN2

#define PWM_FREQ_HZ 2000
#define START_DUTY_CYCLE 0

#define N_FAULT PA28_PIO // nFault pin
#define N_SLEEP PA29_PIO // nSleep pin

uint32_t duty_cycle_right = 0;
uint32_t duty_cycle_left = 0;

int duty_cycle_right_forwards = 0;
int duty_cycle_right_backwards = 0;
int duty_cycle_left_forwards = 0;
int duty_cycle_left_backwards = 0;

pwm_t pwm1;
pwm_t pwm2;
pwm_t pwm3;
pwm_t pwm4;

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

void motor_init(void) 
{
    pio_config_set(N_FAULT, PIO_INPUT);
    pio_config_set(N_FAULT, PIO_OUTPUT_HIGH);
    pio_config_set(N_SLEEP, PIO_OUTPUT_HIGH);
    pio_output_high(N_SLEEP);

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

    //Set PWM to 0
    pwm_duty_set(pwm1, PWM_DUTY_DIVISOR(PWM_FREQ_HZ, 0));
    pwm_duty_set(pwm2, PWM_DUTY_DIVISOR(PWM_FREQ_HZ, 0));
    pwm_duty_set(pwm3, PWM_DUTY_DIVISOR(PWM_FREQ_HZ, 0));
    pwm_duty_set(pwm4, PWM_DUTY_DIVISOR(PWM_FREQ_HZ, 0));
}   


void set_duty(int duty_cycle_left,int duty_cycle_right) 
{
    int duty_cycle_right_forwards = 0;
    int duty_cycle_right_backwards = 0;
    int duty_cycle_left_forwards = 0;
    int duty_cycle_left_backwards = 0;
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

