#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

#define NUM_TASKS 5

// Task structure
typedef struct {
    int task_id;
    int dependencies[NUM_TASKS]; // Array to track dependencies
    int dependency_count;       // Number of unsatisfied dependencies
} Task;

// Shared data
Task tasks[NUM_TASKS];
pthread_mutex_t task_mutex;
pthread_cond_t task_cond;



// Function to simulate task execution. You are not allowed to modify this
void execute_task(int task_id) {
    printf("Task %d is starting...\n", task_id);
    sleep(1); // Simulate task execution time
    printf("Task %d is completed.\n", task_id);
}

// Thread function for each task
// You are allowed to modify only this function
void* task_function(void* arg) {
    int task_id = *(int*)arg;

    pthread_mutex_lock(&task_mutex);
    while(tasks[task_id].dependency_count > 0)
    {
        pthread_cond_wait(&task_cond, &task_mutex);
    }
    pthread_mutex_unlock(&task_mutex);

    // Execute the task
    execute_task(task_id);

    // Notify dependent tasks
    pthread_mutex_lock(&task_mutex);
    for(int i=0; i< NUM_TASKS; i++)
    {
        if(tasks[i].dependencies[task_id])
        {
            tasks[i].dependency_count--;
            if(tasks[i].dependency_count == 0)
            {
                pthread_cond_broadcast(&task_cond);
            }
        }
    }
    pthread_mutex_unlock(&task_mutex);
    free(arg);
    return NULL;
}

// Main function. You are not allowed to modify this.
int main() {
    pthread_t threads[NUM_TASKS];
    pthread_mutex_init(&task_mutex, NULL);
    pthread_cond_init(&task_cond, NULL);

    // Initialize tasks and dependencies
    for (int i = 0; i < NUM_TASKS; i++) {
        tasks[i].task_id = i;
        tasks[i].dependency_count = 0;
        for (int j = 0; j < NUM_TASKS; j++) {
            tasks[i].dependencies[j] = 0;
        }
    }

    // Define task dependencies (example)
    tasks[1].dependencies[0] = 1; // Task 1 depends on Task 0
    tasks[2].dependencies[1] = 1; // Task 2 depends on Task 1
    tasks[3].dependencies[1] = 1; // Task 3 depends on Task 1
    tasks[4].dependencies[2] = 1; // Task 4 depends on Task 2
    tasks[4].dependencies[3] = 1; // Task 4 depends on Task 3

    // Update dependency counts
    for (int i = 0; i < NUM_TASKS; i++) {
        for (int j = 0; j < NUM_TASKS; j++) {
            if (tasks[i].dependencies[j]) {
                tasks[i].dependency_count++;
            }
        }
    }

    // Create threads for tasks
    for (int i = 0; i < NUM_TASKS; i++) {
        int* task_id = malloc(sizeof(int));
        *task_id = i;
        pthread_create(&threads[i], NULL, task_function, task_id);
    }

    // Wait for all threads to complete
    for (int i = 0; i < NUM_TASKS; i++) {
        pthread_join(threads[i], NULL);
    }

    // Cleanup
    pthread_mutex_destroy(&task_mutex);
    pthread_cond_destroy(&task_cond);

    printf("All tasks completed.\n");
    return 0;
}