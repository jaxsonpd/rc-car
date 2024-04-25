/** 
 * @file CTR_control.h
 * @author Jack Duignan (Jdu80@uclive.ac.nz)
 * @date 2024-04-25
 * @brief Interface for the accelerometer proccessing module 
 */


#ifndef CONTROL_H
#define CONTROL_H


#include <stdint.h>
#include <stdbool.h>

#include "adxl345.h"


typedef struct {
    int16_t raw_x;
    int16_t raw_y;
    int16_t raw_z;
} controlData_t;

/** 
 * @brief Initalise the control module including the accelerometer
 * @param slave_addr the address of the adxl345
 * 
 * @return 0 if succesful, 1 if twi fails, 2 if adxl345 fails
 */
int8_t CTR_init (twi_slave_addr_t slave_addr);

/** 
 * @brief Update the control struct with new information
 * @param controlData a pointer to the control struct
 * 
 * @return 0 if succesful, 1 if device was not ready, 2 if read failed
 */
int8_t CTR_update (controlData_t *controlData);


#endif // CONTROL_H