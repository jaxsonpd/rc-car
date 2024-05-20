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
#include "button.h"

#define NUM_LEDS 20
#define BATTERY_VOLTAGE_ADC ADC_CHANNEL_6

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

    // 33k pull down & 47k pull up gives a scale factor or
    // 33 / (47 + 33) = 0.4125
    // 4096 (max ADC reading) * 0.4125 ~= 1690
    return (uint16_t) ((int)s) * 3300 / 1690;
}

void low_battery(void) 
{
    uint8_t leds[NUM_LEDS * 3];
    int i;
    for (i = 0; i < NUM_LEDS; i++)
    {
        // Set full green  GRB order
        leds[i * 3] = 0;
        leds[i * 3 + 1] = 255;
        leds[i * 3 + 2] = 0;
    }
    ledtape_write (LEDTAPE_PIO, leds, NUM_LEDS*3);  
}