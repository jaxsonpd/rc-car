/** 
 * @file communications.c
 * @author Ethan Wildash-Chan (ewi69@uclive.ac.nz)
 * @date 2024-05-01
 * @brief Communications module for both radio and serial
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


static nrf24_t *g_nrf;



// initial radio function
int8_t radio_init (void) {
    spi_cfg_t spi_cfg = {
        .channel = 0,
        .clock_speed_kHz = 1000,
        .cs = RADIO_CS_PIO,
        .mode = SPI_MODE_0,
        .cs_mode = SPI_CS_MODE_FRAME,
        .bits = 8,
    };

    nrf24_cfg_t nrf24_cfg = {
        .channel = RADIO_CHANNEL,
        .address = RADIO_ADDRESS,
        .payload_size = RADIO_PAYLOAD_SIZE,
        .ce_pio = RADIO_CE_PIO,
        .irq_pio = RADIO_IRQ_PIO,
        .spi = spi_cfg,
    };

    pio_config_set (RADIO_POWER_ENABLE_PIO, PIO_OUTPUT_HIGH);
    pio_config_set (RADIO_CONFIG_0, PIO_PULLUP);
    pio_config_set (RADIO_CONFIG_1, PIO_PULLUP);

    uint8_t button_cfg = pio_input_get(RADIO_CONFIG_0) << 1 
        | pio_input_get (RADIO_CONFIG_1);

    switch (button_cfg) {
        case 0:
            nrf24_cfg.channel = 1;
            break;
        case 1:
            nrf24_cfg.channel = 2;
            break;
        case 2:
            nrf24_cfg.channel = 3;
            break;
        case 3:
            nrf24_cfg.channel = 4;
            break;
    }

    g_nrf = nrf24_init(&nrf24_cfg);

    if (! g_nrf) {
        return 1; // error in nrf initialisation
    } else {
        return 0; // successful initialisation
    }
}

// Send message
int8_t radio_tx (radio_packet_t *packet, bool report_tx) {
    char buffer[RADIO_PAYLOAD_SIZE + 1];
    uint8_t num_bytes = 0;

    snprintf(buffer, sizeof (buffer), "%d,%d,%d", 
        packet->right_duty, packet->left_duty, packet->parity);

    num_bytes =  nrf24_write (g_nrf, buffer, RADIO_PAYLOAD_SIZE);

    if (report_tx) {
        printf("tx: \"%d,%d,%d,\" bytes:%d\n", 
        packet->right_duty, packet->left_duty, packet->parity, num_bytes);
    }


    if (! (num_bytes))
        return 0; // Success
    else
        return 1; // Failure
}

int8_t serial_tx (radio_packet_t *packet) {
    printf("%d,%d,%d\n", packet->right_duty, packet->left_duty, packet->parity);

    return 0;
}
    
// Receive message
int16_t radio_rx (void) {
    return 0;
}
