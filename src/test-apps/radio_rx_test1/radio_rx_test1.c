/* File:   radio_rx_test1.c
   Author: M. P. Hayes, UCECE
   Date:   24 Feb 2018
*/
#include "nrf24.h"
#include "spi.h"
#include "usb_serial.h"
#include "pio.h"
#include "delay.h"
#include "panic.h"

#define RADIO_CHANNEL 4
#define RADIO_ADDRESS 0x7284570293LL
#define RADIO_PAYLOAD_SIZE 32

int main(void)
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
            // If the IRQ pin is not connected, use a value of 0.
            // .irq_pio = 0,
            .irq_pio = RADIO_IRQ_PIO,
            .spi = spi_cfg,
        };
    nrf24_t *nrf;

    // Configure LED PIO as output.
    pio_config_set (LED_ERROR_PIO, !LED_ACTIVE);
    pio_config_set (LED_STATUS_PIO, LED_ACTIVE);

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

    while(1)
    {
        char buffer[RADIO_PAYLOAD_SIZE + 1];
        uint8_t bytes;

        bytes = nrf24_read (nrf, buffer, RADIO_PAYLOAD_SIZE);
        if (bytes != 0)
        {
            buffer[bytes] = 0;
            printf ("%s, %d\n", buffer, bytes);
            pio_output_toggle (LED_STATUS_PIO);
        }

        delay_ms (100);
    }
}
