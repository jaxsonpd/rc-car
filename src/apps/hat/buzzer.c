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

uint32_t tune_length = 51; 
char g_tune_test[] = "FDGGXXFDGGXXDFDFXGFDEdEXXFDGGXGbAXDFDFXGFXDEXAGGXXX";
// char g_tune_test[] = "DDDAAaGGFFDFGCC";

static const pwm_cfg_t g_buzzer_pwm_cfg = 
{
    .pio = BUZZER_PIO,
    .period = PWM_PERIOD_DIVISOR (262),
    .duty = PWM_DUTY_DIVISOR (262, 50),
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



/** 
 * @brief Convert the note to a frequency only supports 4th octiv atm 
 * @param note the note to play A->G - lower case indicates flat
 * 
 * @return the frequency of the note in hz
 */
static uint32_t note_to_freq(char note) {
    static uint32_t prev_freq = 262;
    uint32_t freq = 262;
    switch (note) {
        case 'a':
            freq = 415;
            break;
        case 'A':
            freq = 440;
            break;
        case 'b':
            freq = 466;
            break;
        case 'B':
            freq = 494;
            break;
        case 'C':
            freq = 523;
            break;
        case 'd':
            freq = 554;
            break;
        case 'D':
            freq = 587;
            break;
        case 'e':
            freq = 622;
            break;
        case 'E':
            freq = 659;
            break;
        case 'F':
            freq = 698;
            break;
        case 'g':
            freq = 740;
            break;
        case 'G':
            freq = 783;
            break;
        default:
            freq = prev_freq;
            break;
    }
    prev_freq = freq;
    return freq;
}


void buzzer_update(void) {

    static uint32_t note_count = 0;
    static bool prev_zero = false;

    note_count ++;
    if (note_count > tune_length) {
        note_count = 0;
    }

    char note = g_tune_test[note_count];

    if (note == 'X') {
        pwm_duty_set(g_buzzer_pwm, PWM_DUTY_DIVISOR(262, 0));
        prev_zero = true;
    } else {
        if (prev_zero) {
            pwm_duty_set(g_buzzer_pwm, PWM_DUTY_DIVISOR(262, 90));
            prev_zero = false;
        }

        pwm_frequency_set(g_buzzer_pwm, note_to_freq(note));
    }
    

    printf("Set: %c\n", note);
}

void buzzer_off(void) {
    pwm_stop(g_buzzer_pwm);
}