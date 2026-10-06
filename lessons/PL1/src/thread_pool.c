#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define MAX_TASKS 100

typedef struct {
    int number;
} Task;

typedef struct {
    Task tasks[MAX_TASKS];
    int front;
    int rear;
    int count;
    pthread_mutex_t mutex;
    pthread_cond_t cond_nonempty;
    pthread_cond_t cond_nonfull;
} TaskQueue;

TaskQueue taskQueue;
int num_threads;
int work_done = 0;

// ----------- Queue functions -----------
void init_queue(TaskQueue *queue) {
    queue->front = 0;
    queue->rear = 0;
    queue->count = 0;
    pthread_mutex_init(&queue->mutex, NULL);
    pthread_cond_init(&queue->cond_nonempty, NULL);
    pthread_cond_init(&queue->cond_nonfull, NULL);
}

void enqueue(TaskQueue *queue, Task task) {
    pthread_mutex_lock(&queue->mutex);
    while (queue->count == MAX_TASKS) {
        pthread_cond_wait(&queue->cond_nonfull, &queue->mutex);
    }
    queue->tasks[queue->rear] = task;
    queue->rear = (queue->rear + 1) % MAX_TASKS;
    queue->count++;
    pthread_cond_signal(&queue->cond_nonempty);
    pthread_mutex_unlock(&queue->mutex);
}

int dequeue(TaskQueue *queue, Task *task) {
    pthread_mutex_lock(&queue->mutex);
    while (queue->count == 0 && !work_done) {
        pthread_cond_wait(&queue->cond_nonempty, &queue->mutex);
    }
    if (queue->count == 0 && work_done) {
        pthread_mutex_unlock(&queue->mutex);
        return 0;
    }
    *task = queue->tasks[queue->front];
    queue->front = (queue->front + 1) % MAX_TASKS;
    queue->count--;
    pthread_cond_signal(&queue->cond_nonfull);
    pthread_mutex_unlock(&queue->mutex);
    return 1;
}

// ----------- Example task -----------
long long factorial(int n) {
    if (n <= 1) return 1;
    return n * factorial(n - 1);
}

// ----------- Worker thread function -----------
void *worker_function(void *arg) {
    Task task;
    while (1) {
        if (!dequeue(&taskQueue, &task)) {
            break;
        }
        printf("Thread %ld processing task: factorial(%d)\n", pthread_self(), task.number);
        long long result = factorial(task.number);
        printf("Thread %ld result: %d! = %lld\n", pthread_self(), task.number, result);
    }
    printf("Thread %ld exiting.\n", pthread_self());
    return NULL;
}

// ----------- Main program -----------
int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <num_threads> <num_tasks>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    num_threads = atoi(argv[1]);
    int num_tasks = atoi(argv[2]);
    pthread_t threads[num_threads];

    init_queue(&taskQueue);

    // Create worker threads
    for (int i = 0; i < num_threads; i++) {
        pthread_create(&threads[i], NULL, worker_function, NULL);
    }

    // Generate tasks
    for (int i = 0; i < num_tasks; i++) {
        Task t;
        t.number = (rand() % 12) + 1; // factorial of numbers 1–12
        enqueue(&taskQueue, t);
        // Optional: sleep to simulate delay between task arrivals
        usleep(100000);
    }

    // Indicate no more work
    pthread_mutex_lock(&taskQueue.mutex);
    work_done = 1;
    pthread_cond_broadcast(&taskQueue.cond_nonempty);
    pthread_mutex_unlock(&taskQueue.mutex);

    // Wait for all threads to finish
    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("All threads have finished. Exiting program.\n");
    return 0;
}
