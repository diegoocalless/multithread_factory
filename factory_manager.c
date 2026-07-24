/*
 *
 * factory_manager.c
 *
*/ 

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stddef.h>
#include <semaphore.h>
#include <pthread.h>
#include <sys/stat.h>
#include <string.h>
#include <ctype.h>

// Declare external process_manager function 
extern int process_manager(int id, int belt_size, int items_to_produce);

// Structure to hold belt configuration 
struct belt_config {
    int id;
    int size;
    int items;
};

// Structure to pass arguments to process_manager threads 
struct pm_args {
    int id;
    int belt_size;
    int items_to_produce;
};

// Global variables 
sem_t *process_manager_semaphore;
int max_processes;

// Function to read an integer from a buffer starting at position 
int read_int(const char *buffer, int *position, int max_len) {
    int value = 0;
    int i = *position;
    int sign = 1;
    
    // Skip whitespace 
    while (i < max_len && isspace(buffer[i])) {
        i++;
    }
    
    // Check for negative sign 
    if (i < max_len && buffer[i] == '-') {
        sign = -1;
        i++;
    }
    
    // Read digits 
    int has_digits = 0;
    while (i < max_len && isdigit(buffer[i])) {
        value = value * 10 + (buffer[i] - '0');
        i++;
        has_digits = 1;
    }
    
    // Update position 
    *position = i;
    
    // Return value or error if no digits were found 
    return has_digits ? value * sign : -1;
}

// Thread function to execute process manager 
void *run_process_manager(void *arg) {
    struct pm_args *args = (struct pm_args *)arg;
    int result;
    
    // Wait for the semaphore before proceeding 
    sem_wait(process_manager_semaphore);
    
    // Execute the process manager 
    result = process_manager(args->id, args->belt_size, args->items_to_produce);
    
    // Release the semaphore to allow next process manager to run 
    sem_post(process_manager_semaphore);
    
    // Return result code 
    pthread_exit((void*)(intptr_t)result);
}

int main(int argc, const char *argv[]){
    int fd;
    int num_processes = 0;
    int i, rc;
    struct belt_config *belts = NULL;
    pthread_t *threads = NULL;
    struct pm_args *args = NULL;
    void *thread_result;
    char buffer[4096]; // Buffer for file reading 
    ssize_t bytes_read;
    
    // Check command line arguments 
    if (argc != 2) {
        fprintf(stderr, "[ERROR][factory_manager] Invalid file.\n");
        return -1;
    }
    
    // Open the input file
    fd = open(argv[1], O_RDONLY);
    if (fd < 0) {
        fprintf(stderr, "[ERROR][factory_manager] Invalid file.\n");
        return -1;
    }
    
    // Read file content into buffer 
    bytes_read = read(fd, buffer, sizeof(buffer) - 1);
    if (bytes_read <= 0) {
        fprintf(stderr, "[ERROR][factory_manager] Invalid file.\n");
        close(fd);
        return -1;
    }
    
    // Add null terminator to buffer 
    buffer[bytes_read] = '\0';
    
    // Close the file 
    close(fd);
    
    // Parse buffer to get max_processes 
    int pos = 0;
    max_processes = read_int(buffer, &pos, bytes_read);
    if (max_processes <= 0) {
        fprintf(stderr, "[ERROR][factory_manager] Invalid file.\n");
        return -1;
    }
    
    // Allocate memory for belt configurations 
    belts = (struct belt_config *)malloc(max_processes * sizeof(struct belt_config));
    if (belts == NULL) {
        fprintf(stderr, "[ERROR][factory_manager] Invalid file.\n");
        return -1;
    }
    
    // Parse buffer to get belt configurations 
    while (pos < bytes_read && num_processes < max_processes) {
        int id = read_int(buffer, &pos, bytes_read);
        if (id < 0) break;
        
        int size = read_int(buffer, &pos, bytes_read);
        if (size <= 0) {
            fprintf(stderr, "[ERROR][factory_manager] Invalid file.\n");
            free(belts);
            return -1;
        }
        
        int items = read_int(buffer, &pos, bytes_read);
        if (items <= 0) {
            fprintf(stderr, "[ERROR][factory_manager] Invalid file.\n");
            free(belts);
            return -1;
        }
        
        belts[num_processes].id = id;
        belts[num_processes].size = size;
        belts[num_processes].items = items;
        num_processes++;
    }

    //Check for extra data after expected belts
    while (pos < bytes_read) {
        if (!isspace(buffer[pos])) {
            fprintf(stderr, "[ERROR][factory_manager] Invalid file.\n");
            free(belts);
            return -1;
        }
        pos++;
    }

    // Check if we have at least one belt 
    if (num_processes == 0) {
        fprintf(stderr, "[ERROR][factory_manager] Invalid file.\n");
        free(belts);
        return -1;
    }
    
    // Initialize semaphore for process manager synchronization 
    process_manager_semaphore = (sem_t *)malloc(sizeof(sem_t));
    if (process_manager_semaphore == NULL) {
        fprintf(stderr, "[ERROR][factory_manager] Invalid file.\n");
        free(belts);
        return -1;
    }
    
    // Initialize semaphore with value 1 for sequential execution 
    if (sem_init(process_manager_semaphore, 0, 1) != 0) {
        fprintf(stderr, "[ERROR][factory_manager] Invalid file.\n");
        free(process_manager_semaphore);
        free(belts);
        return -1;
    }
    
    // Allocate memory for threads and arguments 
    threads = (pthread_t *)malloc(num_processes * sizeof(pthread_t));
    args = (struct pm_args *)malloc(num_processes * sizeof(struct pm_args));
    
    if (threads == NULL || args == NULL) {
        fprintf(stderr, "[ERROR][factory_manager] Invalid file.\n");
        sem_destroy(process_manager_semaphore);
        free(process_manager_semaphore);
        free(belts);
        if (threads) free(threads);
        if (args) free(args);
        return -1;
    }
    
    // Create process manager threads 
    for (i = 0; i < num_processes; i++) {
        args[i].id = belts[i].id;
        args[i].belt_size = belts[i].size;
        args[i].items_to_produce = belts[i].items;
        
        rc = pthread_create(&threads[i], NULL, run_process_manager, (void *)&args[i]);
        if (rc) {
            fprintf(stderr, "[ERROR][factory_manager] Invalid file.\n");
            sem_destroy(process_manager_semaphore);
            free(process_manager_semaphore);
            free(belts);
            free(threads);
            free(args);
            return -1;
        }
        
        printf("[OK][factory_manager] Process_manager with id %d has been created.\n", belts[i].id);
    }
    
    // Wait for all threads to complete 
    for (i = 0; i < num_processes; i++) {
        rc = pthread_join(threads[i], &thread_result);
        if (rc || (intptr_t)thread_result != 0) {
            fprintf(stderr, "[ERROR][factory_manager] Process_manager with id %d has finished with errors.\n", belts[i].id);
        } else {
            printf("[OK][factory_manager] Process_manager with id %d has finished.\n", belts[i].id);
        }
    }
    
    // Clean up resources 
    sem_destroy(process_manager_semaphore);
    free(process_manager_semaphore);
    free(belts);
    free(threads);
    free(args);
    
    printf("[OK][factory_manager] Finishing.\n");
    
    return 0;
}

