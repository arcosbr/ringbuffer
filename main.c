/******************************************************************************
 *                                                                            *
 *                        Ring Buffer Example Project                         *
 *                                                                            *
 * File: main.c                                                               *
 * Description:                                                               *
 *     Demonstrates the usage of a thread-safe ring buffer by                 *
 *     enqueueing and dequeueing elements at 1-second intervals.              *
 *                                                                            *
 * Author: Anderson Costa                                                     *
 * Date: 2024-11-27                                                           *
 *                                                                            *
 ******************************************************************************/

#include <stdio.h>
#include <stdint.h>
#include <pthread.h>
#include <unistd.h> /* For the sleep function */
#include "ring_buffer.h"

/* Buffer definitions */
#define BUFFER_LENGTH 10U
#define ELEMENT_SIZE sizeof(int)
#define TOTAL_ELEMENTS 16U

/* Structure to pass parameters to threads */
typedef struct
{
    ring_buffer_t *ring_buffer;
    pthread_mutex_t *mutex;
    uint32_t max_elements;
} thread_params_t;

/**
 * @brief Enqueue thread function.
 *
 * This function runs in a separate thread and attempts to enqueue a specified
 * number of elements into the ring buffer at 1-second intervals. It locks the
 * mutex before accessing the ring buffer to ensure thread safety.
 *
 * @param args Pointer to the thread parameters.
 * @return void* No return value.
 */
void *enqueue_thread(void *args)
{
    thread_params_t *params = (thread_params_t *)args;
    ring_buffer_t *ring = params->ring_buffer;
    pthread_mutex_t *mutex = params->mutex;
    uint32_t i;

    for (i = 0; i < params->max_elements; i++)
    {
        int element = (int)i;
        ring_buffer_status_t status;

        /* Lock the mutex before accessing the ring buffer */
        pthread_mutex_lock(mutex);
        status = ring_buffer_push(ring, &element);

        if (status == RING_BUFFER_OK)
        {
            printf("[Enqueue] Pushed element: %d\n", element);
        }
        else if (status == RING_BUFFER_FULL)
        {
            printf("[Enqueue] Failed to push element %d: Buffer is full.\n",
                   element);
        }
        else
        {
            printf("[Enqueue] Failed to push element %d: Error code %d.\n",
                   element, status);
        }

        /* Unlock the mutex after accessing the ring buffer */
        pthread_mutex_unlock(mutex);

        /* Wait for 1 second */
        sleep(1);
    }

    return NULL;
}

/**
 * @brief Dequeue thread function.
 *
 * This function runs in a separate thread and attempts to dequeue elements from
 * the ring buffer at 1-second intervals. It continues until all enqueued
 * elements have been dequeued. It locks the mutex before accessing the ring
 * buffer to ensure thread safety.
 *
 * @param args Pointer to the thread parameters.
 * @return void* No return value.
 */
void *dequeue_thread(void *args)
{
    thread_params_t *params = (thread_params_t *)args;
    ring_buffer_t *ring = params->ring_buffer;
    pthread_mutex_t *mutex = params->mutex;
    int element;
    ring_buffer_status_t status;

    /* Continue dequeuing while there are elements or more can be enqueued */
    while (1)
    {
        /* Lock the mutex before accessing the ring buffer */
        pthread_mutex_lock(mutex);
        status = ring_buffer_pop(ring, &element);

        if (status == RING_BUFFER_OK)
        {
            printf("[Dequeue] Popped element: %d\n", element);
        }
        else if (status == RING_BUFFER_EMPTY)
        {
            /* Check if all elements have been enqueued and dequeued */
            if (ring->head == ring->tail)
            {
                pthread_mutex_unlock(mutex);
                break; /* Exit the loop if all elements have been processed */
            }
            else
            {
                printf("[Dequeue] Buffer is empty.\n");
            }
        }
        else
        {
            printf("[Dequeue] Failed to pop element: Error code %d.\n", status);
        }

        /* Unlock the mutex after accessing the ring buffer */
        pthread_mutex_unlock(mutex);

        /* Wait for 1 second */
        sleep(1);
    }

    return NULL;
}

int main(void)
{
    ring_buffer_t ring_buffer;
    uint8_t buffer[BUFFER_LENGTH * ELEMENT_SIZE];
    ring_buffer_status_t status;
    pthread_mutex_t mutex;
    pthread_t enqueue_tid, dequeue_tid;
    thread_params_t params;

    /* Initialize the mutex */
    if (pthread_mutex_init(&mutex, NULL) != 0)
    {
        printf("Failed to initialize mutex.\n");
        return -1;
    }

    /* Initialize the ring buffer */
    status =
        ring_buffer_init(&ring_buffer, ELEMENT_SIZE, BUFFER_LENGTH, buffer);

    if (status != RING_BUFFER_OK)
    {
        printf("Failed to initialize ring buffer. Status: %d\n", status);
        pthread_mutex_destroy(&mutex);
        return -1;
    }

    printf("Ring buffer initialized with capacity %u.\n", BUFFER_LENGTH);

    /* Set up the parameters for the threads */
    params.ring_buffer = &ring_buffer;
    params.mutex = &mutex;
    params.max_elements = TOTAL_ELEMENTS;

    /* Create the enqueue thread */
    if (pthread_create(&enqueue_tid, NULL, enqueue_thread, &params) != 0)
    {
        printf("Failed to create enqueue thread.\n");
        ring_buffer_destroy(&ring_buffer);
        pthread_mutex_destroy(&mutex);
        return -1;
    }

    /* Create the dequeue thread */
    if (pthread_create(&dequeue_tid, NULL, dequeue_thread, &params) != 0)
    {
        printf("Failed to create dequeue thread.\n");
        pthread_cancel(enqueue_tid);
        ring_buffer_destroy(&ring_buffer);
        pthread_mutex_destroy(&mutex);
        return -1;
    }

    /* Wait for the threads to finish */
    pthread_join(enqueue_tid, NULL);
    pthread_join(dequeue_tid, NULL);

    /* Display the final state of the ring buffer */
    status = ring_buffer_state(&ring_buffer);

    if (status == RING_BUFFER_FULL)
    {
        printf("Final ring buffer state: FULL\n");
    }
    else if (status == RING_BUFFER_EMPTY)
    {
        printf("Final ring buffer state: EMPTY\n");
    }
    else
    {
        printf("Final ring buffer state: OK (Elements in buffer)\n");
    }

    /* Destroy the ring buffer */
    status = ring_buffer_destroy(&ring_buffer);

    if (status != RING_BUFFER_OK)
    {
        printf("Failed to destroy ring buffer. Status: %d\n", status);
        pthread_mutex_destroy(&mutex);
        return -1;
    }

    printf("Ring buffer destroyed successfully.\n");

    /* Destroy the mutex */
    pthread_mutex_destroy(&mutex);

    return 0;
}
