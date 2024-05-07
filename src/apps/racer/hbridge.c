/* File:   hbridhe.c
   Author: I Patel & D Hawes
   Date:   2 May 2024
   Descr:  Runs all 4 PWM signals to H bridge 
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
#include "usb_serial.h"
#include "pwm.h"
#include "delay.h"
#include "panic.h"

#include "target.h"

#define PWM1_PIO PA25_PIO //AIN1
#define PWM2_PIO PB14_PIO //BIN1
#define PWM3_PIO PB1_PIO //AIN2
#define PWM4_PIO PB0_PIO //BIN2

#define N_FAULT PA28_PIO // nFault pin
#define N_SLEEP PA29_PIO // nSleep pin

#define DELAY_MS 10


// If you are using PWM to drive a motor you will need
// to choose a lower frequency!
#define PWM_FREQ_HZ 1000
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

void set_duty(pwm_t pwm1, pwm_t pwm2, pwm_t pwm3, pwm_t pwm4, int duty_cycle_forwards) {
    // Turn on AIN1 (PWM1) and AIN2 (PWM3), and set BIN1 (PWM2) to maximum and BIN2 (PWM4) to minimum
    pwm_duty_set(pwm1, PWM_DUTY_DIVISOR(PWM_FREQ_HZ, duty_cycle_forwards));
    pwm_duty_set(pwm2, PWM_DUTY_DIVISOR(PWM_FREQ_HZ, duty_cycle_forwards));
    pwm_duty_set(pwm3, PWM_DUTY_DIVISOR(PWM_FREQ_HZ, 0));
    pwm_duty_set(pwm4, PWM_DUTY_DIVISOR(PWM_FREQ_HZ, 0));
}

int main (void)
{
    pwm_t pwm1;
    pwm_t pwm2;
    pwm_t pwm3;
    pwm_t pwm4;

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

    pwm_duty_set(pwm1, PWM_DUTY_DIVISOR(PWM_FREQ_HZ, 20));
    pwm_duty_set(pwm2, PWM_DUTY_DIVISOR(PWM_FREQ_HZ, 20));
    pwm_duty_set(pwm3, PWM_DUTY_DIVISOR(PWM_FREQ_HZ, 0));
    pwm_duty_set(pwm4, PWM_DUTY_DIVISOR(PWM_FREQ_HZ, 0));

    while (1)
    {
        delay_ms(DELAY_MS);
        char buf[256];
        if (fgets(buf, sizeof(buf), stdin)) {
            int num;
            if (sscanf(buf, "%d", &num) == 1) {
                if (num >= -100 && num <= 100) {
                    printf("You entered: %d\n", num);
                    set_duty(pwm1, pwm2, pwm3, pwm4, num);
                }
                else {
                    printf("Please enter -100 - 100 No.\n");
                }
            } else {
                printf("Invalid input\n");
                // Clear input buffer
                // while (getchar() != '\n');
            }
        }
    }

    return 0;
}
