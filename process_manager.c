#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stddef.h>
#include <pthread.h>
#include "queue.h"
#include <semaphore.h>

#define NUM_THREADS 2

// Structure to pass arguments to producer and consumer threads 
struct thread_args {
    int belt_id;
    int items_to_produce;
    int *produced;
    int *consumed;
};

// Producer thread function 
void *producer_function(void *arg) {
    struct thread_args *args = (struct thread_args *)arg;
    int belt_id = args->belt_id;

    for (int i = 0; i < args->items_to_produce; i++) {
        struct element new_elem;
        new_elem.num_edition = i;
        new_elem.id_belt = belt_id;
        new_elem.last = (i == args->items_to_produce - 1) ? 1 : 0;

        if (queue_put(&new_elem) < 0) {
            fprintf(stderr, "[ERROR][process_manager] There was an error executing process_manager with id %d.\n", belt_id);
            pthread_exit((void*)-1);
        }

        (*args->produced)++;
    }

    pthread_exit(NULL);
}

// Consumer thread function 
void *consumer_function(void *arg) {
    struct thread_args *args = (struct thread_args *)arg;
    int belt_id = args->belt_id;

    for (int i = 0; i < args->items_to_produce; i++) {
        struct element *elem = queue_get();
        if (elem == NULL) {
            fprintf(stderr, "[ERROR][process_manager] There was an error executing process_manager with id %d.\n", belt_id);
            pthread_exit((void*)-1);
        }

        (*args->consumed)++;
        free(elem);
    }

    pthread_exit(NULL);
}

int process_manager(int id, int belt_size, int items_to_produce) {
    pthread_t threads[NUM_THREADS];
    struct thread_args args;
    void *thread_result;
    int rc;

    // Individual counters for each production
    int elements_produced = 0;
    int elements_consumed = 0;

    printf("[OK][process_manager] Process_manager with id %d waiting to produce %d elements.\n", id,items_to_produce);

    if (queue_init(belt_size) < 0) {
        fprintf(stderr, "[ERROR][process_manager] There was an error executing process_manager with id %d.\n", id);
        return -1;
    }

    queue_set_id(id);

    printf("[OK][process_manager] Belt with id %d has been created with a maximum of %d elements.\n", id, belt_size);

    args.belt_id = id;
    args.items_to_produce = items_to_produce;
    args.produced = &elements_produced;
    args.consumed = &elements_consumed;

    rc = pthread_create(&threads[0], NULL, producer_function, (void *)&args);
    if (rc) {
        fprintf(stderr, "[ERROR][process_manager] There was an error executing process_manager with id %d.\n", id);
        queue_destroy();
        return -1;
    }

    rc = pthread_create(&threads[1], NULL, consumer_function, (void *)&args);
    if (rc) {
        fprintf(stderr, "[ERROR][process_manager] There was an error executing process_manager with id %d.\n", id);
        pthread_cancel(threads[0]);
        queue_destroy();
        return -1;
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        rc = pthread_join(threads[i], &thread_result);
        if (rc || thread_result != NULL) {
            fprintf(stderr, "[ERROR][process_manager] There was an error executing process_manager with id %d.\n",id);
            queue_destroy();
            return -1;
        }
    }

    if (elements_produced != items_to_produce || elements_consumed != items_to_produce) {
        fprintf(stderr, "[ERROR][process_manager] There was an error executing process_manager with id %d.\n", id);
        queue_destroy();
        return -1;
    }

    printf("[OK][process_manager] Process_manager with id %d has produced %d elements.\n", id, items_to_produce);

    if (queue_destroy() < 0) {
        fprintf(stderr, "[ERROR][process_manager] There was an error executing process_manager with id %d.\n", id);
        return -1;
    }

    return 0;
}
