/* File:   battery.c
   Author: I Patel, D Hawes
   Date:   May 2024
   Descr:  File for testing battery V 
*/

#ifndef BATTERY_H
#define BATTERY_H

#include <stdio.h>
#include "usb_serial.h"
#include "nrf24.h"
#include "adc.h"
#include "pio.h"
#include "pacer.h"
#include "panic.h"
#include "delay.h"
#include "button.h"

#define NUM_LEDS 20
#define BATTERY_VOLTAGE_ADC ADC_CHANNEL_6

static int battery_sensor_init (void);

uint16_t battery_millivolts (void);

void low_battery(void); 

#endif