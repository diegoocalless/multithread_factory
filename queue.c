#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include "queue.h"

// Global queue structure 
static struct queue* q = NULL;

// Initialize the queue with the specified size 
int queue_init(int size) {
    if (size <= 0) {
        return -1;
    }

    // Allocate memory for the queue structure
    q = (struct queue*)malloc(sizeof(struct queue));
    if (q == NULL) {
        return -1;
    }

    //Allocate memory for the elements array
    q->elements = (struct element**)malloc(size * sizeof(struct element*));
    if (q->elements == NULL) {
        free(q);
        q = NULL;
        return -1;
    }

    // Initialize the queue properties 
    q->size = size;
    q->count = 0;
    q->head = 0;
    q->tail = 0;

    //Initialize synchronization mechanisms
    if (pthread_mutex_init(&q->mutex, NULL) != 0) {
        free(q->elements);
        free(q);
        q = NULL;
        return -1;
    }
    
    if (pthread_cond_init(&q->not_empty, NULL) != 0) {
        pthread_mutex_destroy(&q->mutex);
        free(q->elements);
        free(q);
        q = NULL;
        return -1;
    }
    
    if (pthread_cond_init(&q->not_full, NULL) != 0) {
        pthread_cond_destroy(&q->not_empty);
        pthread_mutex_destroy(&q->mutex);
        free(q->elements);
        free(q);
        q = NULL;
        return -1;
    }

    return 0;
}

//Add an element to the queue
int queue_put(struct element* elem) {
    if (q == NULL || elem == NULL) {
        fprintf(stderr, "[ERROR][queue] There was an error while using queue with id: %d.\n", 
               (q != NULL) ? q->id : -1);
        return -1;
    }
    
    //Lock the mutex
    pthread_mutex_lock(&q->mutex);
    
    // Wait until the queue has space 
    while (q->count == q->size) {
        pthread_cond_wait(&q->not_full, &q->mutex);
    }
    
    // Copy element to preserve it
    struct element* new_elem = (struct element*)malloc(sizeof(struct element));
    if (new_elem == NULL) {
        pthread_mutex_unlock(&q->mutex);
        fprintf(stderr, "[ERROR][queue] There was an error while using queue with id: %d.\n", q->id);
        return -1;
    }
    
    memcpy(new_elem, elem, sizeof(struct element));
    
    //add the element to the queue
    q->elements[q->tail] = new_elem;
    q->tail = (q->tail + 1) % q->size;
    q->count++;
    
    
    printf("[OK][queue] Introduced element with id %d in belt %d.\n", 
           new_elem->num_edition, new_elem->id_belt);
    
    //Signal that queue is not empty
    pthread_cond_signal(&q->not_empty);
    
    //Unlock the mutex
    pthread_mutex_unlock(&q->mutex);
    
    return 0;
}

//Get an element from the queue
struct element* queue_get(void) {
    if (q == NULL) {
        fprintf(stderr, "[ERROR][queue] There was an error while using queue with id: %d.\n", -1);
        return NULL;
    }
    
    // Lock the mutex 
    pthread_mutex_lock(&q->mutex);
    
    // Wait until the queue has elements 
    while (q->count == 0) {
        pthread_cond_wait(&q->not_empty, &q->mutex);
    }
    
    // Get the element from the queue 
    struct element* elem = q->elements[q->head];
    q->head = (q->head + 1) % q->size;
    q->count--;
    
    printf("[OK][queue] Obtained element with id %d in belt %d.\n", 
           elem->num_edition, elem->id_belt);
    
    // Signal that queue is not full 
    pthread_cond_signal(&q->not_full);
    
    //Unlock the mutex
    pthread_mutex_unlock(&q->mutex);
    
    return elem;
}

// Check if the queue is empty
int queue_empty(void) {
    if (q == NULL) {
        return 1;
    }
    
    pthread_mutex_lock(&q->mutex);
    int empty = (q->count == 0);
    pthread_mutex_unlock(&q->mutex);
    
    return empty;
}

// Check if the queue is full 
int queue_full(void) {
    if (q == NULL) {
        return 0;
    }
    
    pthread_mutex_lock(&q->mutex);
    int full = (q->count == q->size);
    pthread_mutex_unlock(&q->mutex);
    
    return full;
}

//Destroy the queue and free resources
int queue_destroy(void) {
    if (q == NULL) {
        return -1;
    }
    
    //Free any remaining elements
    pthread_mutex_lock(&q->mutex);
    
    for (int i = 0; i < q->count; i++) {
        int idx = (q->head + i) % q->size;
        free(q->elements[idx]);
    }
    
    free(q->elements);
    
    pthread_mutex_unlock(&q->mutex);
    
    //Destroy synchronization objects
    pthread_cond_destroy(&q->not_empty);
    pthread_cond_destroy(&q->not_full);
    pthread_mutex_destroy(&q->mutex);
    
    //Free queue structure
    free(q);
    q = NULL;
    
    return 0;
}

int queue_set_id(int id){
  if (q==NULL) return -1;
  q->id=id;
  return 0;
}
