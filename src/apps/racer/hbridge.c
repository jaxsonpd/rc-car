/* File:   pwm_test2.c
   Author: M. P. Hayes, UCECE
   Date:   15 April 2013
   Descr:  This example starts two channels simultaneously; one inverted
           with respect to the other.
*/
// #include <stdint.h>
// #include <stdbool.h>

// #include <pio.h>
// #include "pwm.h"
// #include "delay.h"
// #include "panic.h"

// #include "target.h"

#include <stdint.h>
#include <stdbool.h>

#include <pio.h>

#include "pwm.h"
#include "delay.h"
#include "panic.h"

#include "target.h"

#define PWM1_PIO PA25_PIO //AIN1
#define PWM2_PIO PB14_PIO //BIN1
#define PWM3_PIO PB1_PIO //AIN2
#define PWM4_PIO PB0_PIO //BIN2


// If you are using PWM to drive a motor you will need
// to choose a lower frequency!
#define PWM_FREQ_HZ 20000
#define START_DUTY_CYCLE 0

uint32_t duty_cycle = 50;

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

int
main (void)
{
    pwm_t pwm1;
    pwm_t pwm2;
    pwm_t pwm3;
    pwm_t pwm4;

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

    while (1)
    {
        // delay_ms (5000);
        // if(duty_cycle >= 50) {
        //     duty_cycle -= 20;
        //     pwm_duty_set(pwm1, PWM_DUTY_DIVISOR(PWM_FREQ_HZ, duty_cycle));
        // } else {
        //     duty_cycle += 20;
        //     pwm_duty_set(pwm1, PWM_DUTY_DIVISOR(PWM_FREQ_HZ, duty_cycle));
        // }
        // pio_output_toggle (LED_STATUS_PIO);

        // Turn on AIN1 (PWM1) and AIN2 (PWM3), and set BIN1 (PWM2) to maximum and BIN2 (PWM4) to minimum
        pwm_duty_set(pwm1, PWM_DUTY_DIVISOR(PWM_FREQ_HZ, 80));
        pwm_duty_set(pwm2, PWM_DUTY_DIVISOR(PWM_FREQ_HZ, 80));
        pwm_duty_set(pwm3, PWM_DUTY_DIVISOR(PWM_FREQ_HZ, 0));
        pwm_duty_set(pwm4, PWM_DUTY_DIVISOR(PWM_FREQ_HZ, 0));
        delay_ms(5000); // Wait for 5 seconds

        // Turn on BIN1 (PWM2) and BIN2 (PWM4), and set AIN1 (PWM1) to maximum and AIN2 (PWM3) to minimum
        pwm_duty_set(pwm1, PWM_DUTY_DIVISOR(PWM_FREQ_HZ, 0));
        pwm_duty_set(pwm2, PWM_DUTY_DIVISOR(PWM_FREQ_HZ, 0));
        pwm_duty_set(pwm3, PWM_DUTY_DIVISOR(PWM_FREQ_HZ, 80));
        pwm_duty_set(pwm4, PWM_DUTY_DIVISOR(PWM_FREQ_HZ, 80));
        delay_ms(5000); // Wait for 5 seconds
    }

    return 0;
}
