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

/***
 * @struct radio_config_t
 * 
 * @var radio_config_t::config_0
 *      Bit to indicate position of radio config switch 0
 * @var radio_config_t::config_1
 *      Bit to indicate position of radio config switch 1
 *
*/  

typedef struct {
    int8_t config_0;
    int8_t config_1;
} radio_config_t;


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
void radio_tx (int16_t right_duty, int16_t left_duty, int16_t parity);

/** 
 * @brief Recieve a packet over radio
 * @return Packet recieved or Error
 */
int16_t radio_rx (void);

/** 
 * @brief Configures the radio based on the radio configuration switches
 * @returns The switch positions of the radio dip switches
 */
int8_t radio_config (void);




