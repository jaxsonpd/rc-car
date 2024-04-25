/** 
 * @file CTR_control.c
 * @author Jack Duignan (Jdu80@uclive.ac.nz)
 * @date 2024-04-25
 * @brief Accelerometer processing implementation
 * 
 * This file implements both accelerometer proccessing and converting the 
 * raw data to control inputs to be sent over radio.
 *
 * Accelerometer Settings:
 * FIFO_CTL 0x80 - stream mode FIFO holds last 32 values and overwrites
 * POWER_CTL 0x08 - measure bit
 * DATA_FORMAT 0x08 - Full resolution mode +-2g (4mg/LSB scale)
 */


#include <stdint.h>
#include <stdbool.h>

#include "adxl345.h"

#include "CTR_control.h"

static twi_t adxl345_twi;
static adxl345_t *adxl345;


static twi_cfg_t adxl345_twi_cfg =
{
    .channel = TWI_CHANNEL_0,
    .period = TWI_PERIOD_DIVISOR (100000), // 100 kHz
    .slave_addr = 0
};


int8_t CTR_init (twi_slave_addr_t slave_addr) {
    adxl345_twi = twi_init(&adxl345_twi_cfg);

    if (!adxl345_twi) return 1; // twi failed

    adxl345 = adxl345_init (adxl345_twi, slave_addr);

    if (!adxl345) return 2; // adx failed

    return 0;
}


static int8_t get_raw_data (int16_t accel[3]) {
    if (! adxl345_is_ready (adxl345)) return 1; // Not ready
    
    if (adxl345_accel_read (adxl345, accel)) return 0; // Complete
    else return 2; // Read Failed
}


int8_t CTR_update (controlData_t *controlData) {
    int16_t accel[3];
    
    int8_t r_readResult = get_raw_data (accel);

    if (r_readResult == 0) {
        controlData->raw_x = accel[0];
        controlData->raw_y = accel[1];
        controlData->raw_z = accel[2];
    }

    return r_readResult;
}
