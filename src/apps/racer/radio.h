/* File:   radio.h
   Author: I Patel & D Hawes
   Date:   9 May 2024
   Descr:  Recieves radio signal and writes to PWM 
*/

#ifndef RADIO_H
#define RADIO_H

#define TX_LED PA1_PIO  //STAT0 Blue
#define RX_LED PA0_PIO //STAT1  Red

// initialise radio transmission and configure pins
void radio_init(void);

//receives information from hat and returns duty cycle values
int radio_rx(char* buffer);

// transmits hit signal
int radio_tx(uint8_t hit_signal);

#endif