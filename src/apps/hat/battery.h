/** 
 * @file battery.h
 * @author Jack Duignan (Jdu80@uclive.ac.nz)
 * @date 2024-05-21
 * @brief Battery low voltage detection header
 */


#ifndef BATTERY_H
#define BATTERY_H


#include <stdint.h>
#include <stdbool.h>

/** 
 * @brief Initialise the adc battery
 * 
 * @return 0 if succesful
 */
int battery_sensor_init (void);

/** 
 * @brief Return the battery voltage in millivolts
 * 
 * @return the battery voltage in millivolts
 */
uint16_t battery_millivolts (void);



#endif // BATTERY_H