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

#define RADIO_CHANNEL 4              // Needs to match car communications
#define RADIO_ADDRESS 0x0123456789LL // Needs to match car communications
#define RADIO_PAYLOAD_SIZE 32

static spi_cfg_t spi_cfg = {
        .channel = 0,
        .clock_speed_kHz = 1000,
        .cs = RADIO_CS_PIO,
        .mode = SPI_MODE_0
        .cs_mode = SPI_CS_MODE_FRAME,
        .bits = 8,
    }

static nrf24_cfg_t nrf24_cfg = {
        .channel = RADIO_CHANNEL,
        .address = RADIO_ADDRESS,
        .payload_size = RADIO_PAYLOAD_SIZE,
        .ce_pio = RADIO_CE_PIO,
        .irq_pio = RADIO_IRQ_PIO,
        .spi = spi_cfg,
    }

// initial radio function
int8_t radio_init (void) {
    pio_config_set (RADIO_POWER_ENABLE_PIO, PIO_OUTPUT_HIGH);
    pio_config_set (RADIO_CONFIG_0, PIO_PULLDOWN);
    pio_config_set (RADIO_CONFIG_1, PIO_PULLDOWN);

    nrf = nrf_init(&nrf24_cfg);
    if (! nrf) 
        return 1; // error in nrf initialisation
    else
        return 0; // successful intialisation

}

// Send message
void radio_tx (int16_t right_duty, int16_t left_duty, int16_t parity) {
    char buffer[RADIO_PAYLOAD_SIZE + 1]

    snprintf(buffer, sizeof (buffer), "%d,%d,%d", right_duty, left_duty, parity);

    // writes to nrf radio???
    if (! nrf24_write (nrf, buffer, RADIO_PAYLOAD_SIZE))  
        return 0;
    else
        return 1;
}
    
// Recieve message
int16_t radio_rx (void) {
    char buffer[RADIO_PAYLOAD_SIZE + 1];
    uint8_t bytes = nrf24_rad (nrf, buffer, RADIO_PAYLOAD_SIZE);

    // if message is not empty, print to terminal
    if (bytes != 0) {
        buffer [bytes] = 0;
        // do something with recieved message (probably not needed for hat)
    }
}

//Configure Radio
radio_config_t radio_config (void) {
    radio_config_t g_radio_config;
    g_radio_config.config_0 = pio_input_get(RADIO_CONFIG_0);
    g_radio_config.config_1 = pio_input_get(RADIO_CONFIG_1);
    return g_radio_config;
    
}
