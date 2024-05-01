/** 
 * @file circular_buffer.h
 * @author Jack Duignan (Jdu80@uclive.ac.nz)
 * @date 2024-04-26
 * @brief Interface for the int32_t circular buffer 
 */


#ifndef CIRCULAR_BUFFER_H
#define CIRCULAR_BUFFER_H


#include <stdint.h>
#include <stdbool.h>

/**
 * @struct circ_buff_t
 * A struct to store the circular buffers values
 * 
 * @var circ_buff_t::size
 *      The size of the circular buffer
 * @var circ_buff_t::windex
 *      The write index
 * @var circ_buff_t::rindex
 *      The read index
 * @var circ_buff_t::data
 *      A pointer to the data
*/
typedef struct {
    uint32_t size;
    uint32_t windex;
    uint32_t rindex;
    int32_t *data;
} circ_buff_t;


/** 
 * @brief Initialise the circular buffer
 * 
 * Sets both indices to the start of the buffer
 * @param circ_buff the circular buffer struct to initialise
 * @param size the size of the buffer   
 * 
 * @return 0 if successful
 */
int8_t circ_buff_init (circ_buff_t *buffer, int32_t size);


/** 
 * @brief Write to the circular buffer
 * 
 * Advances the write index
 * @param buffer the circular buffer to write too
 * @param entry the entry to write
 */
void circ_buff_write (circ_buff_t *buffer, int32_t entry);


/** 
 * @brief Read from a circular buffer
 * @param buffer the circular buffer to read from
 * 
 * @return the value found at the rindex
 */
int32_t circ_buff_read (circ_buff_t *buffer);


/** 
 * @brief Find the mean of the circular buffer
 * 
 * Currently only uses int32_t for a totalizer so use with care
 * @param buffer the circular buffer to find the mean of
 * 
 * @return the mean of the data in the buffer
 */
int32_t circ_buff_mean(circ_buff_t *buffer);


/** 
 * @brief Free the memory allocated to a circular buffer
 * @param buffer the buffer to free
 * 
 */
void circ_buff_free (circ_buff_t *buffer);



#endif // CIRCULAR_BUFFER_H