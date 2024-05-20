/* File:   battery.c
   Author: I Patel, D Hawes
   Date:   May 2024
   Descr:  File for testing battery V 
*/
#include <stdio.h>
#include "usb_serial.h"
#include "nrf24.h"
#include "adc.h"
#include "pio.h"
#include "pacer.h"
#include "ledtape.h"
#include "panic.h"
#include "delay.h"

#define NUM_LEDS 20
#define BATTERY_VOLTAGE_ADC ADC_CHANNEL_6
#define LED_ERROR PA0_PIO // red error LED

static adc_t battery_sensor;

static int battery_sensor_init (void)
{
    adc_cfg_t bat =
        {
            .channel = BATTERY_VOLTAGE_ADC,
            .bits = 12,
            .trigger = ADC_TRIGGER_SW,
            .clock_speed_kHz = F_CPU / 4000,
        };

    battery_sensor = adc_init (&bat);

    return (battery_sensor == 0) ? -1 : 0;
}

uint16_t battery_millivolts (void)
{
    adc_sample_t s;

    adc_read (battery_sensor, &s, sizeof (s));

    // 68k pull down & 100k pull up gives a scale factor or
    // 68 / (100 + 68) = 0.4048
    // 4096 (max ADC reading) * 0.4048 ~= 1658
    return (uint16_t) ((int)s) * 3300 / 1658;
}

void low_battery(void) 
{
    while (battery_millivolts () < 5000)
        {
            pio_output_toggle (LED_ERROR);
            delay_ms (200);
        }
  
}