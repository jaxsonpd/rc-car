/* File:   radio_tx_test1.c
   Author: M. P. Hayes, UCECE
   Date:   24 Feb 2018
*/
#include "nrf24.h"
#include "pio.h"
#include "pacer.h"
#include "usb_serial.h"
#include "stdio.h"
#include "delay.h"
#include "panic.h"

#define RADIO_CHANNEL 1
#define RADIO_ADDRESS 0x0123456789LL
#define RADIO_PAYLOAD_SIZE 32

int main (void)
{
    spi_cfg_t spi_cfg =
        {
            .channel = 0,
            .clock_speed_kHz = 1000,
            .cs = RADIO_CS_PIO,
            .mode = SPI_MODE_0,
            .cs_mode = SPI_CS_MODE_FRAME,
            .bits = 8
        };
    nrf24_cfg_t nrf24_cfg =
        {
            .channel = RADIO_CHANNEL,
            .address = RADIO_ADDRESS,
            .payload_size = RADIO_PAYLOAD_SIZE,
            .ce_pio = RADIO_CE_PIO,
            .irq_pio = RADIO_IRQ_PIO,
            .spi = spi_cfg,
        };
    uint8_t count = 0;
    nrf24_t *nrf;

    // Configure LED PIO as output.
    pio_config_set (LED_ERROR_PIO, !LED_ACTIVE);
    pio_config_set (LED_STATUS_PIO, LED_ACTIVE);
    pacer_init (10);

    // Redirect stdio to USB serial.
    usb_serial_stdio_init ();

#ifdef RADIO_POWER_ENABLE_PIO
    // Enable radio regulator if present.
    pio_config_set (RADIO_POWER_ENABLE_PIO, PIO_OUTPUT_HIGH);
    delay_ms (10);
#endif

    nrf = nrf24_init (&nrf24_cfg);
    if (! nrf)
        panic (LED_ERROR_PIO, 2);

    while (1)
    {
        char buffer[RADIO_PAYLOAD_SIZE + 1];

        pacer_wait ();
        pio_output_toggle (LED_STATUS_PIO);

        snprintf (buffer, sizeof (buffer), "Hello world %d\r\n", count++);
        printf("Tx: %s\n", buffer);
        if (! nrf24_write (nrf, buffer, RADIO_PAYLOAD_SIZE))
            pio_output_set (LED_ERROR_PIO, 0);
        else
            pio_output_set (LED_ERROR_PIO, 1);
    }
    delay_ms(500);
}
