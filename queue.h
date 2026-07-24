#ifndef HEADER_FILE
#define HEADER_FILE

#include <pthread.h>

struct element {
  int num_edition;
  int id_belt;
  int last;
};

//Circular queue structure 
struct queue {
  struct element** elements;  // Array to store elements 
  int size;                  // Maximum capacity of queue 
  int count;                 // Current number of elements 
  int head;                  // Index of the first element 
  int tail;                  // Index where next element will be inserted 
  int id;                    // Belt ID for this queue 
  pthread_mutex_t mutex;     // Mutex for thread synchronization 
  pthread_cond_t not_empty;  // Condition variable for queue not empty
  pthread_cond_t not_full;   // Condition variable for queue not full 
};

// Queue management functions 
int queue_init(int size);
int queue_destroy(void);
int queue_put(struct element* elem);
struct element* queue_get(void);
int queue_empty(void);
int queue_full(void);
int queue_set_id(int id);

#endif
