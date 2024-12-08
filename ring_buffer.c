/******************************************************************************
 *                                                                            *
 *                         Ring Buffer Implementation                         *
 *                                                                            *
 * File: ring_buffer.c                                                        *
 * Description:                                                               *
 *     Implementation of the ring buffer functions for managing a             *
 *     thread-safe circular buffer.                                           *
 *                                                                            *
 * Author: Anderson Costa                                                     *
 * Date: 2024-11-27                                                           *
 *                                                                            *
 ******************************************************************************/

#include "ring_buffer.h"
#include <string.h>
#include <stdint.h>

/* Inline function to calculate the next index in the ring buffer */
static inline uint32_t ring_buffer_next_index(uint32_t index, uint32_t lenght)
{
	return  (index + 1U) % lenght ;

}

/* Inline Function to calculate the previous index in the ring buffer */
static inline uint32_t ring_buffer_prev_index(uint32_t index, uint32_t lenght)
{
	return (index == 0U) ? (lenght - 1U) : (index - 1U);
}


/* Initialize the ring buffer */
ring_buffer_status_t ring_buffer_init(ring_buffer_t *handle,
                                      size_t element_size, uint32_t length,
                                      uint8_t *buffer)
{
    if ((handle == NULL) || (buffer == NULL))
    {
        return RING_BUFFER_INVALID_PARAMS;
    }

    if (handle->init_flag == RING_BUFFER_INITIALIZE_MASK)
    {
        return RING_BUFFER_ALREADY_INITIALIZED;
    }

    if ((element_size == 0U) || (length < 2U))
    {
        return RING_BUFFER_INVALID_PARAMS;
    }

    /* Clear the handle structure */
    (void)memset(handle, 0, sizeof(ring_buffer_t));

    handle->element_size = element_size;
    handle->length = length;
    handle->buffer = buffer;

    handle->is_dynamic = 0U;
    handle->is_empty = 1U;
    handle->init_flag = RING_BUFFER_INITIALIZE_MASK;

    return RING_BUFFER_OK;
}

/* Destroy the ring buffer */
ring_buffer_status_t ring_buffer_destroy(ring_buffer_t *handle)
{
    if (handle == NULL)
    {
        return RING_BUFFER_INVALID_PARAMS;
    }

    if (handle->init_flag != RING_BUFFER_INITIALIZE_MASK)
    {
        return RING_BUFFER_NOT_INITIALIZED;
    }

    /* No dynamic memory allocation, so no need to free buffer */
    /* Clear the handle structure */
    (void)memset(handle, 0, sizeof(ring_buffer_t));

    return RING_BUFFER_OK;
}

/* Push an element into the ring buffer */
ring_buffer_status_t ring_buffer_push(ring_buffer_t *handle,
                                      const void *element)
{
    if ((handle == NULL) || (element == NULL))
    {
        return RING_BUFFER_INVALID_PARAMS;
    }

    if (handle->init_flag != RING_BUFFER_INITIALIZE_MASK)
    {
        return RING_BUFFER_NOT_INITIALIZED;
    }

    if (handle->is_full != 0U)
    {
        return RING_BUFFER_FULL;
    }
	
    /* Calculate the offset for the head position */
    uint32_t offset = handle->head * handle->element_size;

    /* Copy the element into the buffer at the head position */
    (void)memcpy(&handle->buffer[offset], element, handle->element_size);

	/* Update the head position*/
	handle->head = ring_buffer_next_index(handle->head, handle->length);

	/* Check if buffer is now full */
	if(handle->head == handle->tail)
	{
		handle->is_full = 1U ;
	}
   	
	handle->is_empty = 0U ;
 
    return RING_BUFFER_OK;
}

/* Pop an element from the ring buffer */
ring_buffer_status_t ring_buffer_pop(ring_buffer_t *handle, void *element)
{
    if ((handle == NULL) || (element == NULL))
    {
        return RING_BUFFER_INVALID_PARAMS;
    }

    if (handle->init_flag != RING_BUFFER_INITIALIZE_MASK)
    {
        return RING_BUFFER_NOT_INITIALIZED;
    }

    if (handle->is_empty != 0U)
    {
        return RING_BUFFER_EMPTY;
    }

    /* Calculate the offset for the tail position */
    uint32_t offset = handle->tail * handle->element_size;

    /* Copy the element from the buffer at the tail position */
    (void)memcpy(element, &handle->buffer[offset], handle->element_size);

	/* Update the tail position */
	handle->tail = ring_buffer_next_index(handle->tail, handle->length);

	/* Check if buffer is now empty */
	if (handle->head == handle->tail)	
	{
		handle->is_empty = 1U ;
	}

	handle->is_full = 0U ;

    return RING_BUFFER_OK;
}

/* Get the current state of the ring buffer */
ring_buffer_status_t ring_buffer_state(const ring_buffer_t *handle)
{
    if (handle == NULL)
    {
        return RING_BUFFER_INVALID_PARAMS;
    }

    if (handle->init_flag != RING_BUFFER_INITIALIZE_MASK)
    {
        return RING_BUFFER_NOT_INITIALIZED;
    }

    if (handle->is_empty != 0U)
    {
        return RING_BUFFER_EMPTY;
    }
    else if (handle->is_full != 0U)
    {
        return RING_BUFFER_FULL;
    }

    return RING_BUFFER_OK;
}

/* Clear the ring buffer */
ring_buffer_status_t ring_buffer_clear(ring_buffer_t *handle)
{
    if (handle == NULL)
    {
        return RING_BUFFER_INVALID_PARAMS;
    }

    if (handle->init_flag != RING_BUFFER_INITIALIZE_MASK)
    {
        return RING_BUFFER_NOT_INITIALIZED;
    }

    /* Clear the buffer memory */
    (void)memset(handle->buffer, 0, handle->length * handle->element_size);

    /* Reset the state flags and indices */
    handle->head = 0U;
    handle->tail = 0U;
    handle->is_full = 0U;
    handle->is_empty = 1U;

    return RING_BUFFER_OK;
}
