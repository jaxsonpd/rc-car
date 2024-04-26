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
} control_data_t;

/** 
 * @brief Initialise the control module including the accelerometer
 * @param slave_addr the address of the adxl345
 * @param buff_en sets wether the circular buffers are enabled
 * 
 * @return 0 if successful, 1 if twi fails, 2 if adxl345 fails
 */
int8_t control_init (twi_slave_addr_t slave_addr, bool buff_en);


/** 
 * @brief Update the circular buffers for the control module if necessary
 * 
 * @return 0 if successful, 1 if device was not ready, 2 if read failed, 
 * 3 if circular buffers not enabled
 */
int8_t control_update (void);

/** 
 * @brief Update the control struct with new information
 * @param controlData a pointer to the control struct
 * 
 * @return 0 if successful, 1 if device was not ready, 2 if read failed
 */
int8_t control_get_data (control_data_t *control_data);

#endif // CONTROL_H
