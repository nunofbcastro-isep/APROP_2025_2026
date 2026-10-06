#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// To run for a single barber (part a), set NUM_BARBERS to 1.
#define NUM_BARBERS 2
#define NUM_CHAIRS 5
#define MAX_CUSTOMERS 20

// Semaphores
sem_t customers; // Number of customers waiting for a haircut
sem_t barbers;   // Number of barbers waiting for customers
pthread_mutex_t mutex; // For protecting access to 'waiting'

int waiting = 0; // Customers waiting in the waiting room

void* barber_function(void* arg) {
    int barber_id = *(int*)arg;
    free(arg);

    while (1) {
        // The barber waits for a customer to arrive.
        // sem_wait on 'customers' makes the barber sleep if the count is 0.
        printf("Barber %d is sleeping.\n", barber_id);
        fflush(stdout);
        sem_wait(&customers);

        // Lock the mutex to safely decrement the number of waiting customers.
        pthread_mutex_lock(&mutex);
        waiting--;
        pthread_mutex_unlock(&mutex);

        // Signal that a barber is ready to cut hair.
        sem_post(&barbers);

        // The barber is cutting hair.
        printf("Barber %d is cutting hair. Waiting customers: %d\n", barber_id, waiting);
        fflush(stdout);
        sleep(rand() % 3 + 1); // Simulate haircut time
    }
    return NULL;
}

void* customer_function(void* arg) {
    int customer_id = *(int*)arg;
    free(arg);

    sleep(rand() % 5 + 1); // Customer arrives at a random time

    // Lock the mutex to check if there are available chairs.
    pthread_mutex_lock(&mutex);
    printf("Customer %d arrived. Waiting customers: %d\n", customer_id, waiting);
    fflush(stdout);

    if (waiting < NUM_CHAIRS) {
        // There is a free chair in the waiting room.
        waiting++;
        pthread_mutex_unlock(&mutex);

        // Wake up a barber by posting to the 'customers' semaphore.
        sem_post(&customers);

        // The customer waits for an available barber.
        // sem_wait on 'barbers' makes the customer wait if the count is 0.
        sem_wait(&barbers);

        printf("Customer %d is getting a haircut.\n", customer_id);
        fflush(stdout);
    } else {
        // The waiting room is full, so the customer leaves.
        pthread_mutex_unlock(&mutex);
        printf("Customer %d is leaving, waiting room is full.\n", customer_id);
        fflush(stdout);
    }
    return NULL;
}

int main() {
    pthread_t barber_threads[NUM_BARBERS];
    pthread_t customer_threads[MAX_CUSTOMERS];

    // Initialize semaphores and mutex
    sem_init(&customers, 0, 0);
    sem_init(&barbers, 0, 0);
    pthread_mutex_init(&mutex, NULL);

    printf("--- Sleeping Barber Problem ---\n");
    printf("Barbers: %d, Waiting Chairs: %d, Customers: %d\n\n", NUM_BARBERS, NUM_CHAIRS, MAX_CUSTOMERS);

    // Create barber threads
    for (int i = 0; i < NUM_BARBERS; i++) {
        int* id = malloc(sizeof(int));
        *id = i + 1;
        pthread_create(&barber_threads[i], NULL, barber_function, id);
    }

    // Create customer threads
    for (int i = 0; i < MAX_CUSTOMERS; i++) {
        int* id = malloc(sizeof(int));
        *id = i + 1;
        pthread_create(&customer_threads[i], NULL, customer_function, id);
        sleep(1); // Stagger customer arrivals
    }

    // Join customer threads
    for (int i = 0; i < MAX_CUSTOMERS; i++) {
        pthread_join(customer_threads[i], NULL);
    }

    // In this simulation, we'll let the customers finish and then stop.
    printf("\nAll customers have been served or have left.\n");
    // We can cancel the barber threads for a clean exit in this simulation
    for (int i = 0; i < NUM_BARBERS; i++) {
        pthread_cancel(barber_threads[i]);
    }

    // Destroy semaphores and mutex
    sem_destroy(&customers);
    sem_destroy(&barbers);
    pthread_mutex_destroy(&mutex);

    return 0;
}
