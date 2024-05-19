/** 
 * @file buzzer.c
 * @author Ethan Wildash-Chan (ewi69@uclive.ac.nz)
 * @date 2024-04-30
 * @brief Uses the piezo buzzer to play a melody
 * https://github.com/robsoncouto/arduino-songs/blob/master/miichannel/miichannel.ino - website for example songs
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include <pio.h>

#include "pwm.h"

#include "buzzer.h"

#define MAX_FREQ 2000
#define MIN_FREQ 2
#define FREQ_STEP 25

uint32_t tune_length = 48; 
uint16_t g_tune_test[] = {350, 294, 392, 0, 0, 350, 294, 392, 0, 0, 294, 350, 
                        294, 350, 392, 350, 0, 294, 330, 277, 330, 0, 0, 350, 
                        294, 392, 0, 0, 392, 466, 440, 0, 284, 350, 294, 350,
                        0, 392, 350, 294, 330, 440, 392, 440, 392, 330, 0, 0};

static const pwm_cfg_t g_buzzer_pwm_cfg = 
{
    .pio = BUZZER_PIO,
    .period = PWM_PERIOD_DIVISOR (4000),
    .duty = PWM_DUTY_DIVISOR (4000, 50),
    .align = PWM_ALIGN_LEFT,
    .polarity = PWM_POLARITY_HIGH,
    .stop_state = PIO_OUTPUT_LOW
};

static pwm_t g_buzzer_pwm;


int8_t buzzer_init (void) {
    g_buzzer_pwm = pwm_init(&g_buzzer_pwm_cfg);

    if (! g_buzzer_pwm) {
        return 1;
    }

    pwm_start(g_buzzer_pwm);
    return 0;
}


void buzzer_update(void) {
    // static pwm_frequency_t current_freq = MIN_FREQ;

    // current_freq += FREQ_STEP;

    // if (current_freq > MAX_FREQ) {
    //     current_freq = MIN_FREQ;
    // }

    // //pwm_duty_set(g_buzzer_pwm, PWM_DUTY_DIVISOR(4000, current_freq));
    // pwm_frequency_set(g_buzzer_pwm, current_freq);

    // printf("Current freq: %ld\n", (uint32_t)current_freq);

    static uint32_t note_count = 0;

    note_count ++;
    if (note_count > tune_length) {
        note_count = 0;
    }

    uint16_t note = g_tune_test[note_count];

    if (!note == 0) {
        pwm_frequency_set(g_buzzer_pwm, note);
    }
    

    printf("Set: %d", note);
}

