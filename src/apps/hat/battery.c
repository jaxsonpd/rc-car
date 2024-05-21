/** 
 * @file battery.c
 * @author Jack Duignan (Jdu80@uclive.ac.nz)
 * @date 2024-05-21
 * @brief Battery low voltage detection implementation
 */


#include <stdint.h>
#include <stdbool.h>

#include <stdio.h>

#include "adc.h"


static adc_t battery_sensor;

int battery_sensor_init (void)
{
    adc_cfg_t bat =
        {
            .channel = BATTERY_ADC_CHANNEL,
            .bits = 12,
            .trigger = ADC_TRIGGER_SW,
            .clock_speed_kHz = F_CPU / 4000,
        };

    battery_sensor = adc_init (&bat);

    return (battery_sensor == 0) ? 1 : 0;
}

uint16_t battery_millivolts (void)
{
    adc_sample_t s;

    battery_sensor_init();

    adc_read (battery_sensor, &s, sizeof (s));

    // 68k pull down & 100k pull up gives a scale factor or
    // 68 / (100 + 68) = 0.4048
    // 4096 (max ADC reading) * 0.4048 ~= 1658
    return (uint16_t) ((int)s) * 3300 / 1658;
}