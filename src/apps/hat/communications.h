/** 
 * @file communications.h
 * @author Ethan Wildash-Chan (ewi69@uclive.ac.nz)
 * @date 2024-05-01
 * @brief Interface for the communications proccessing module
 */

#ifndef COMMUNICATIONS_H
#define COMMUNICATIONS_H

#include <stdint.h>
#include <stdbool.h>

typedef struct {
    int8_t left_duty;
    int8_t right_duty;
    uint8_t dastardly;
    uint8_t parity;
} radio_packet_t;

/** 
 * @brief Initialise the radio channel and module
 * @return 0 for successful initaliation, 1 if nrf init fails
 */
int8_t radio_init (void);

/** 
 * @brief Transmit a packet over radio
 * @param packet pointer to radio packet for transmission
 * @param report_tx if true will print the tx to serial monitor
 * 
 * @return indicating success or failure of nrf24_write
 */
int8_t radio_tx (radio_packet_t *packet, bool report_tx);

/** 
 * @brief Send the radio packet out on the serial line
 * @param packet the radio packet to send
 * 
 * @return 0 on success
 */
int8_t serial_tx (radio_packet_t *packet);

/** 
 * @brief Receive a packet over radio
 * @return Packet received or Error
 */
int16_t radio_rx (void);

#endif
