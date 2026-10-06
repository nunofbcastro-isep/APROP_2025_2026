#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define NUM_PHILOSOPHERS 5

pthread_mutex_t forks[NUM_PHILOSOPHERS];
int philosopher_ids[NUM_PHILOSOPHERS];

void* philosopher(void* arg) {
    int id = *(int*)arg;
    int left_fork = id;
    int right_fork = (id + 1) % NUM_PHILOSOPHERS;

    // To prevent deadlock, the last philosopher picks up the right fork first.
    if (id == NUM_PHILOSOPHERS - 1) {
        int temp = left_fork;
        left_fork = right_fork;
        right_fork = temp;
    }

    while (1) {
        // Thinking
        printf("Philosopher %d is thinking.\n", id);
        fflush(stdout);
        sleep(rand() % 3 + 1);

        // Hungry
        printf("Philosopher %d is hungry.\n", id);
        fflush(stdout);

        pthread_mutex_lock(&forks[left_fork]);
        printf("Philosopher %d picked up left fork %d.\n", id, left_fork);
        fflush(stdout);

        pthread_mutex_lock(&forks[right_fork]);
        printf("Philosopher %d picked up right fork %d.\n", id, right_fork);
        fflush(stdout);

        // Eating
        printf("Philosopher %d is eating.\n", id);
        fflush(stdout);
        sleep(rand() % 3 + 1);

        // Done eating
        pthread_mutex_unlock(&forks[right_fork]);
        printf("Philosopher %d put down right fork %d.\n", id, right_fork);
        fflush(stdout);
        pthread_mutex_unlock(&forks[left_fork]);
        printf("Philosopher %d put down left fork %d.\n", id, left_fork);
        fflush(stdout);
    }

    return NULL;
}

int main() {
    pthread_t philosophers[NUM_PHILOSOPHERS];

    // Initialize mutexes
    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        pthread_mutex_init(&forks[i], NULL);
        philosopher_ids[i] = i;
    }

    // Create philosopher threads
    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        pthread_create(&philosophers[i], NULL, philosopher, &philosopher_ids[i]);
    }

    // Join philosopher threads (will run forever)
    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        pthread_join(philosophers[i], NULL);
    }

    // Destroy mutexes
    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        pthread_mutex_destroy(&forks[i]);
    }

    return 0;
}
