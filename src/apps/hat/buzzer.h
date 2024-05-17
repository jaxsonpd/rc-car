/** 
 * @file buzzer.h
 * @author Jack Duignan (Jdu80@uclive.ac.nz)
 * @date 2024-05-17
 * @brief The declarations for the buzzer module
 */


#ifndef BUZZER_H
#define BUZZER_H


#include <stdint.h>
#include <stdbool.h>

/** 
 * @brief Initialise the buzzer to allow it to play music
 * 
 * @return 0 if successful
 */
int8_t buzzer_init(void);

/** 
 * @brief Update the buzzer for use in the paced loop
 * 
 */
void buzzer_update(void);


#endif // BUZZER_H