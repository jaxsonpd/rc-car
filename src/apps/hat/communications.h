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
} radio_packet_t;

/** 
 * @brief Initialise the radio module including the accelerometer
 * @return 0 for successful initaliation, 1 if nrf init fails
 */
int8_t radio_init (void);

/** 
 * @brief Transmit a packet over radio
 * @param right_duty The duty cycle to drive the right chassis tread
 * @param left_duty The duty cycle to drive the left chassis trad
 * @param parity The parity bits to confirm communication
 */
void radio_tx (radio_packet_t *packet);

/** 
 * @brief Recieve a packet over radio
 * @return Packet recieved or Error
 */
int16_t radio_rx (void);

#endif
