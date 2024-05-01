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
 * @struct comms_packet
 * 
 * @var comms::right_duty
 *      The PWM duty cycle for the right track
 * @var comms::left_duty
 *      The PWM duty cycle for the left track
 * @var
 * 
 * @var comms::parity
 *      Parity to confirm packet recieved from correct hat. Determined as 
 *      half right_duty + half left_duty
*/  

typedef struct {
    int16_t right_duty;
    int16_t left_duty;
    int16_t parity;
}   comms_packet;

/** 
 * @brief Initialise the radio module including the accelerometer
 */
void radio_init (void);

/** 
 * @brief Collates parameters into a communication packet
 * @param right_duty The duty cycle to drive the right chassis tread
 * @param left_duty The duty cycle to drive the left chassis trad
 * @param parity The parity bits to confirm communication
 * @return A created communication packet
 */
comms_packet comms_packet_setup (int16_t right_duty, int16_t left_duty, int16_t parity);

/** 
 * @brief Transmit a packet over radio
 * @param packet A communications packet
 */
void radio_tx (comms_packet packet);

/** 
 * @brief Recieve a packet over radio
 * @return Packet recieved or Error
 */
comms_packet radio_rx (void);

/** 
 * @brief Configures the radio based on the radio configuration switches
 */
void radio_config (void);




