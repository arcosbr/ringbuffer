/******************************************************************************
 *                                                                            *
 *                         Ring Buffer Implementation                         *
 *                                                                            *
 * File: ring_buffer.h                                                        *
 * Description:                                                               *
 *     Header file defining the ring buffer data structure and                *
 *     function prototypes for managing a thread-safe ring buffer.            *
 *                                                                            *
 * Author: Anderson Costa                                                     *
 * Date: 2024-11-27                                                           *
 *                                                                            *
 ******************************************************************************/

#ifndef RING_BUFFER_H_
#define RING_BUFFER_H_

#include <stdint.h>
#include <stddef.h>

/* Initialization mask to check if the ring buffer has been initialized */
#define RING_BUFFER_INITIALIZE_MASK 0x5DU

/* Enumeration for ring buffer status codes */
typedef enum
{
    RING_BUFFER_OK,
    RING_BUFFER_FAIL,
    RING_BUFFER_INVALID_PARAMS,
    RING_BUFFER_FULL,
    RING_BUFFER_EMPTY,
    RING_BUFFER_NOT_INITIALIZED,
    RING_BUFFER_ALREADY_INITIALIZED,
    RING_BUFFER_UNKNOWN_ERROR = 0xFFU
} ring_buffer_status_t;

/* Structure representing the ring buffer */
typedef struct
{
    uint8_t *buffer;        /* Pointer to the buffer memory */
    uint32_t length;        /* Number of elements in the buffer */
    size_t element_size;    /* Size of each element in bytes */
    uint32_t head;          /* Index of the head (next write position) */
    uint32_t tail;          /* Index of the tail (next read position) */
    uint8_t init_flag;      /* Initialization flag */
    uint8_t is_full : 1;    /* Flag indicating if the buffer is full */
    uint8_t is_empty : 1;   /* Flag indicating if the buffer is empty */
    uint8_t is_dynamic : 1; /* Flag indicating if buffer was dynamically
                               allocated */
} ring_buffer_t;

/* Function prototypes */

/**
 * @brief Initialize the ring buffer.
 *
 * @param handle        Pointer to the ring buffer handle.
 * @param element_size  Size of each element in bytes.
 * @param length        Number of elements in the buffer.
 * @param buffer        Pointer to the buffer memory (must not be NULL).
 * @return ring_buffer_status_t Status code.
 */
ring_buffer_status_t ring_buffer_init(ring_buffer_t *handle,
                                      size_t element_size, uint32_t length,
                                      uint8_t *buffer);

/**
 * @brief Destroy the ring buffer.
 *
 * @param handle Pointer to the ring buffer handle.
 * @return ring_buffer_status_t Status code.
 */
ring_buffer_status_t ring_buffer_destroy(ring_buffer_t *handle);

/**
 * @brief Push an element into the ring buffer.
 *
 * @param handle Pointer to the ring buffer handle.
 * @param element Pointer to the element to be pushed.
 * @return ring_buffer_status_t Status code.
 */
ring_buffer_status_t ring_buffer_push(ring_buffer_t *handle,
                                      const void *element);

/**
 * @brief Pop an element from the ring buffer.
 *
 * @param handle Pointer to the ring buffer handle.
 * @param element Pointer where the popped element will be stored.
 * @return ring_buffer_status_t Status code.
 */
ring_buffer_status_t ring_buffer_pop(ring_buffer_t *handle, void *element);

/**
 * @brief Get the current state of the ring buffer.
 *
 * @param handle Pointer to the ring buffer handle.
 * @return ring_buffer_status_t Status code (RING_BUFFER_EMPTY,
 * RING_BUFFER_FULL, or RING_BUFFER_OK).
 */
ring_buffer_status_t ring_buffer_state(const ring_buffer_t *handle);

/**
 * @brief Clear the ring buffer.
 *
 * @param handle Pointer to the ring buffer handle.
 * @return ring_buffer_status_t Status code.
 */
ring_buffer_status_t ring_buffer_clear(ring_buffer_t *handle);

#endif /* RING_BUFFER_H_ */
