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
 * @brief Read the radio to see if the bumper has been pressed
 * 
 * @return 0 if buffer hasn't been press 1 if it has -1 if there has been an error
 */
int8_t radio_get_bumper(void);

/** 
 * @brief Set the radio to listen mode
 * 
 * @return result of nrf24_listen
 */
bool radio_listen (void);

/** 
 * @brief Power down the radio
 * 
 * @return 0 if successful
 */
bool radio_power_down(void);

/** 
 * @brief Powers up the radio
 * 
 * @return 0 if successful
 */
bool radio_power_up(void);

/** 
 * @brief Check if there is a message waiting
 * 
 * @return 1 if message waiting
 */
bool radio_rx_data_ready(void);


#endif
