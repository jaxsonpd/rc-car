/* File:   radio.c
   Author: I Patel & D Hawes
   Date:   9 May 2024
   Descr:  Recieves radio signal and writes to PWM 
*/
#include "nrf24.h"
#include "spi.h"
#include "usb_serial.h"
#include "pio.h"
#include "delay.h"
#include "panic.h"
#include "stdio.h"
#include "delay.h"

#include "radio.h"

// Defined values from the example code
#define RADIO_CHANNEL 4                 // match with hat
#define RADIO_ADDRESS 0x7284570293LL
#define RADIO_ADDRESS_TEST 0x0123456789LL    // will need to calibrate with hat board
#define RADIO_PAYLOAD_SIZE 32

static nrf24_t *nrf;

int radio_channel_number;

int radio_channel_number_get(void) {
    return radio_channel_number;
}

// initialise radio transmission
void radio_init(void)
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
    

    // Configure Radio pins.
    pio_config_set (RADIO_POWER_ENABLE_PIO, PIO_OUTPUT_HIGH);
    pio_config_set (RADIO_CONFIG_0, PIO_PULLUP);
    pio_config_set (RADIO_CONFIG_1, PIO_PULLUP);
    delay_ms (10);

    // configure pins to identify transmission mode (main use for testing)
    // pio_config_set (TX_LED, PIO_OUTPUT_LOW);
    // pio_config_set (RX_LED, PIO_OUTPUT_LOW);
    // pacer_init (100);

    // usb_serial_stdio_init ();

// compared to hat code, they include this to change the radio config via DIP switch

    uint8_t button_cfg = pio_input_get(RADIO_CONFIG_0) << 1 
        | pio_input_get (RADIO_CONFIG_1);

    switch (button_cfg) {
        case 0:
            nrf24_cfg.channel = 33;
            break;
        case 1:
            nrf24_cfg.channel = 45;
            break;
        case 2:
            nrf24_cfg.channel = 59;
            break;
        case 3:
            nrf24_cfg.channel = 43;
            break;
    }

    radio_channel_number = nrf24_cfg.channel;

    nrf = nrf24_init (&nrf24_cfg);
    // if (! nrf)
    //     panic (LED_ERROR_PIO, 2);

    
    radio_channel_number_get();

}

//receives information from hat and returns duty cycle values
int radio_rx(char* buffer)
{   
    int8_t bytes;

    //reads radio transmission
    bytes = nrf24_read (nrf, buffer, RADIO_PAYLOAD_SIZE);
    if (bytes != 0)
    {
        buffer[bytes] = 0;
        // printf ("RX: %s\n", buffer);
    }
    return bytes; 
}

// triggered by bump detect and will transmit signal to hat, returns 1 when sent
int radio_tx(uint8_t hit_signal)
{
    char buffer[RADIO_PAYLOAD_SIZE + 1];
    // uint8_t hit_signal = 1; //decide what signal to send for 'hit'

    // transmits 'hit' signal
    snprintf (buffer, sizeof (buffer), "%d", hit_signal); 
    // printf("Tx: %s\n", buffer); // used for serial check
    // if (! nrf24_write (nrf, buffer, RADIO_PAYLOAD_SIZE)){
    //     printf("                 RX: %d\n", hit_signal);
    //     return ;
    // } else {
    //     // printf ("TX: Failure\n");
    //     return 0;
    // }
    // delay_ms(500);
    return nrf24_write (nrf, buffer, RADIO_PAYLOAD_SIZE);
}

bool radio_listen(void){
    return nrf24_listen(nrf);
}

bool radio_rx_data_ready(void){
    return nrf24_is_data_ready(nrf);
}

bool radio_power_down(void) {
    bool result = nrf24_power_down(nrf);
    pio_output_low(RADIO_POWER_ENABLE_PIO);
    return !result;
}

bool radio_power_up(void) {
    pio_output_high(RADIO_POWER_ENABLE_PIO);
    return nrf24_power_up(nrf);
}