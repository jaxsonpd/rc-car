/** 
 * @file communications.c
 * @author Ethan Wildash-Chan (ewi69@uclive.ac.nz)
 * @date 2024-05-01
 * @brief Communications 
 * 
 * Build Options:
 * DRIVERS = pacer usb_serial adxl345 panic
 * SRC = control_test.c control.c circular_buffer.c
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include "nrf24.h"
#include "spi.h"
#include "usb_serial.h"
#include "pio.h"
#include "delay.h"
#include "panic.h"

#include "communications.h"

// initial radio function
void radio_init (void) {
    pio_config_set (RADIO_POWER_ENABLE_PIO, pio_output_high)
}

//Packetiser
comms_packet comms_packet_setup (int16_t right_duty, int16_t left_duty, int16_t parity) {

}

// Send message
void radio_tx (comms_packet packet) {

}
    

// Recieve message
comms_packet radio_rx (void) {

}

//Configure Radio
void radio_config (void) {
    
}
