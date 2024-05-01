/** 
 * @file circular_buffer.c
 * @author Jack Duignan (Jdu80@uclive.ac.nz)
 * @date 2024-04-26
 * @brief Implementation of a circular buffer with int32_t values 
 */


#include <stdint.h>
#include <stdbool.h>

#include "stdlib.h"

#include "circular_buffer.h"



int8_t circ_buff_init (circ_buff_t *buffer, int32_t size) {
    buffer->windex = 0;
    buffer->rindex = 0;
    buffer->size = size;
    buffer->data = (int32_t*) calloc(size, sizeof(int32_t));

    return (buffer->data == NULL);
}


void circ_buff_write (circ_buff_t *buffer, int32_t entry) {
    buffer->data[buffer->windex] = entry;
    
    buffer->windex++;
    if (buffer->windex >= buffer->size) {
        buffer->windex = 0;
    }
}


int32_t circ_buff_read (circ_buff_t *buffer) {
    int32_t data = buffer->data[buffer->rindex];
    
    buffer->rindex++;
    if (buffer->rindex >= buffer->size) {
        buffer->rindex = 0;
    }

    return data;
}

int32_t circ_buff_mean(circ_buff_t *buffer) {
    int32_t total = 0;
    for (uint32_t i = 0; i < buffer->size; i++) {
        total += circ_buff_read(buffer);
    }

    return total / buffer->size;

}


void circ_buff_free (circ_buff_t *buffer) {
    buffer->size = 0;
    buffer->windex = 0;
    buffer->rindex = 0;
    
    free(buffer->data);
    buffer->data = NULL;
}
