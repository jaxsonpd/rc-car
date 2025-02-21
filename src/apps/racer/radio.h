/* File:   radio.h
   Author: I Patel & D Hawes
   Date:   9 May 2024
   Descr:  Recieves radio signal and writes to PWM 
*/

#ifndef RADIO_H
#define RADIO_H

#define TX_LED PA1_PIO  //STAT0 Blue
#define RX_LED PA0_PIO //STAT1  Red

int radio_channel_number_get(void);
// initialise radio transmission and configure pins
void radio_init(void);

//receives information from hat and returns duty cycle values
int radio_rx(char* buffer);

// transmits hit signal
int radio_tx(uint8_t hit_signal);

bool radio_listen(void);

bool radio_rx_data_ready(void);

bool radio_power_down(void);

bool radio_power_up(void);

#endif