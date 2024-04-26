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

#include "circular_buffer.h"

#include "control.h"

#define BUFFER_SIZE 8


static twi_t adxl345_twi;
static adxl345_t *adxl345;

circ_buff_t *x_buffer, *y_buffer, *z_buffer;

static twi_cfg_t adxl345_twi_cfg =
{
    .channel = TWI_CHANNEL_0,
    .period = TWI_PERIOD_DIVISOR (100000), // 100 kHz
    .slave_addr = 0
};

bool g_buff_en = false;

int8_t control_init (twi_slave_addr_t slave_addr, bool buff_en) {
    adxl345_twi = twi_init(&adxl345_twi_cfg);

    if (!adxl345_twi) return 1; // twi failed

    adxl345 = adxl345_init (adxl345_twi, slave_addr);

    if (!adxl345) return 2; // adx failed

    if (buff_en) {
        g_buff_en = true;

        circ_buff_init(x_buffer, BUFFER_SIZE);
        circ_buff_init(y_buffer, BUFFER_SIZE);
        circ_buff_init(z_buffer, BUFFER_SIZE);
    }

    return 0;
}


static int8_t get_raw_data (int16_t accel[3]) {
    if (! adxl345_is_ready (adxl345)) return 1; // Not ready
    
    if (adxl345_accel_read (adxl345, accel)) return 0; // Complete
    else return 2; // Read Failed
}


int8_t control_update (void) {
    if (!g_buff_en) return 3;

    int16_t accel[3];
    
    int8_t r_readResult = get_raw_data (accel);

    if (r_readResult == 0) {
        circ_buff_write(x_buffer, accel[0]);
        circ_buff_write(y_buffer, accel[1]);
        circ_buff_write(z_buffer, accel[2]);
    }

    return r_readResult;
}


int8_t control_get_data (control_data_t *control_data) {
    int8_t r_read_result = 0;

    if (g_buff_en) {
        control_data->raw_x = circ_buff_mean(x_buffer);
        control_data->raw_y = circ_buff_mean(y_buffer);
        control_data->raw_z = circ_buff_mean(z_buffer);

    } else {
        int16_t accel[3];
    
        r_read_result = get_raw_data (accel);

        if (r_read_result == 0) {
            control_data->raw_x = accel[0];
            control_data->raw_y = accel[1];
            control_data->raw_z = accel[2];
        }
    }

    return r_read_result;
}
