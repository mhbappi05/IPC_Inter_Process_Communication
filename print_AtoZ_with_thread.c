#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

#define NUM_THREADS 26

pthread_mutex_t lock;                       // Mutex for synchronization
pthread_cond_t cond[NUM_THREADS];           // Array of condition variables
int current_index = 0;                      // Shared variable to track the current thread to execute

// Function for each thread
void* print_letter(void* arg) {
    int index = *(int*)arg;                 // Index of the thread
    free(arg);                              // Free the dynamically allocated memory

    pthread_mutex_lock(&lock);
    while (index != current_index) {        // Wait until it's this thread's turn
        pthread_cond_wait(&cond[index], &lock);
    }

    // Print the corresponding letter
    printf("%c ", 'A' + index);
    fflush(stdout);

    // Signal the next thread
    current_index++;
    if (current_index < NUM_THREADS) {
        pthread_cond_signal(&cond[current_index]);
    }

    pthread_mutex_unlock(&lock);
    return NULL;
}

int main() {
    pthread_t threads[NUM_THREADS];

    // Initialize the mutex and condition variables
    pthread_mutex_init(&lock, NULL);
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_cond_init(&cond[i], NULL);
    }

    // Create threads
    for (int i = 0; i < NUM_THREADS; i++) {
        int* index = malloc(sizeof(int));
        *index = i; // Pass the index to the thread
        if (pthread_create(&threads[i], NULL, print_letter, index) != 0) {
            perror("Failed to create thread");
            exit(1);
        }
    }

    // Signal the first thread to start
    pthread_mutex_lock(&lock);
    pthread_cond_signal(&cond[0]);
    pthread_mutex_unlock(&lock);

    // Wait for all threads to finish
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    // Cleanup
    pthread_mutex_destroy(&lock);
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_cond_destroy(&cond[i]);
    }

    return 0;
}
