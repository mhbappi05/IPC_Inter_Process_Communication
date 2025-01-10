#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

#define NUM_THREADS 26  // Number of letters in the alphabet

pthread_mutex_t lock;
pthread_cond_t cond[NUM_THREADS];
int current_letter = 0;  // To keep track of which thread should print

void* print_letter(void* arg) {
    int thread_id = *(int*)arg;  // Thread ID representing the letter index

    pthread_mutex_lock(&lock);

    // Wait until it's this thread's turn to print
    while (thread_id != current_letter) {
        pthread_cond_wait(&cond[thread_id], &lock);
    }

    // Print the corresponding letter
    printf("%c ", 'A' + thread_id);

    // Signal the next thread
    current_letter++;
    pthread_cond_signal(&cond[current_letter]);

    pthread_mutex_unlock(&lock);

    return NULL;
}

int main() {
    pthread_t threads[NUM_THREADS];
    int thread_ids[NUM_THREADS];  // Array to store thread IDs
    pthread_mutex_init(&lock, NULL);

    // Initialize condition variables
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_cond_init(&cond[i], NULL);
    }

    // Create threads
    for (int i = 0; i < NUM_THREADS; i++) {
        thread_ids[i] = i;  // Assign thread ID
        pthread_create(&threads[i], NULL, print_letter, &thread_ids[i]);
    }

    // Signal the first thread to start
    pthread_mutex_lock(&lock);
    pthread_cond_signal(&cond[0]);
    pthread_mutex_unlock(&lock);

    // Wait for all threads to complete
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    // Cleanup
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_cond_destroy(&cond[i]);
    }
    pthread_mutex_destroy(&lock);

    return 0;
}
