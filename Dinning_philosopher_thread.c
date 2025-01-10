#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>  // For rand()
#include <unistd.h>  // For sleep()

#define NUM_PHILOSOPHERS 5

pthread_mutex_t mutexes[NUM_PHILOSOPHERS];
pthread_cond_t cond_vars[NUM_PHILOSOPHERS];
int forks[NUM_PHILOSOPHERS];

void initialize() {
    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        pthread_mutex_init(&mutexes[i], NULL);
        pthread_cond_init(&cond_vars[i], NULL);
        forks[i] = 1;  // 1 indicates the fork is available
    }
}

void think(int philosopher) {
    printf("Philosopher %d is thinking.\n", philosopher);
    sleep(1 + (rand() % 3));  // Sleep for 1-3 seconds
}

void eat(int philosopher) {
    printf("Philosopher %d is eating.\n", philosopher);
    sleep(1 + (rand() % 3));  // Sleep for 1-3 seconds
}

void pick_up_forks(int philosopher) {
    int left_fork = philosopher;
    int right_fork = (philosopher + 1) % NUM_PHILOSOPHERS;

    pthread_mutex_lock(&mutexes[philosopher]);

    // Wait until both forks are available
    while (forks[left_fork] == 0 || forks[right_fork] == 0) {
        pthread_cond_wait(&cond_vars[philosopher], &mutexes[philosopher]);
    }

    // Pick up the forks
    forks[left_fork] = 0;  // Take the left fork
    forks[right_fork] = 0; // Take the right fork

    printf("Philosopher %d picked up forks.\n", philosopher);

    pthread_mutex_unlock(&mutexes[philosopher]);
}

void put_down_forks(int philosopher) {
    int left_fork = philosopher;
    int right_fork = (philosopher + 1) % NUM_PHILOSOPHERS;

    pthread_mutex_lock(&mutexes[philosopher]);

    // Put down the forks
    forks[left_fork] = 1;  // Release the left fork
    forks[right_fork] = 1; // Release the right fork

    printf("Philosopher %d put down forks.\n", philosopher);

    // Signal neighbors to check if they can now pick up forks
    pthread_cond_signal(&cond_vars[(philosopher + NUM_PHILOSOPHERS - 1) % NUM_PHILOSOPHERS]); // Signal left neighbor
    pthread_cond_signal(&cond_vars[(philosopher + 1) % NUM_PHILOSOPHERS]);                  // Signal right neighbor

    pthread_mutex_unlock(&mutexes[philosopher]);
}

void* philosopher(void* num) {
    int philosopher = *(int*)num;
    while (1) {
        think(philosopher);
        pick_up_forks(philosopher);
        eat(philosopher);
        put_down_forks(philosopher);
    }
    return NULL;
}

int main() {
    pthread_t philosophers[NUM_PHILOSOPHERS];
    int philosopher_numbers[NUM_PHILOSOPHERS];

    initialize();

    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        philosopher_numbers[i] = i;
        pthread_create(&philosophers[i], NULL, philosopher, &philosopher_numbers[i]);
    }

    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        pthread_join(philosophers[i], NULL);
    }

    return 0;
}
