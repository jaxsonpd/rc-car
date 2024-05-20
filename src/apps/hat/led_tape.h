/** 
 * @file led_tape.h
 * @author Jack Duignan (Jdu80@uclive.ac.nz)
 * @date 2024-05-20
 * @brief The Header file for the led tap
 */


#ifndef LED_TAPE_H
#define LED_TAPE_H


#include <stdint.h>
#include <stdbool.h>

/** 
 * @brief Initalise the led Tape
 * 
 * @return 0 if succesful
 */
int8_t led_tape_init(void);

/** 
 * @brief Control the led tap
 * @param stopped 1 if the racer is stopped
 * 
 */
void led_tape_update(bool stopped);

/** 
 * @brief Turn off the led tape
 * 
 */
void led_tape_off(void);



#endif // LED_TAPE_H