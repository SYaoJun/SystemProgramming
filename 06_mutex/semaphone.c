#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define BUFFER_SIZE 5 // Buffer size

int buffer[BUFFER_SIZE]; // Buffer array
int in  = 0;             // Producer write position
int out = 0;             // Consumer read position

sem_t           empty; // Semaphore for empty buffer slots
sem_t           full;  // Semaphore for full buffer slots
pthread_mutex_t mutex; // Mutex to protect the buffer

void* producer(void* arg) {
    int item;
    while (1) {
        item = rand() % 100; // Generate a random number as the produced item
        sem_wait(&empty);    // Wait for an empty slot
        pthread_mutex_lock(&mutex); // Lock the buffer

        buffer[in] = item; // Write item into the buffer
        printf("Produced: %d\n", item);
        in = (in + 1) % BUFFER_SIZE; // Update write position

        pthread_mutex_unlock(&mutex); // Unlock the buffer
        sem_post(&full);              // Increment full-slot semaphore

        sleep(1); // Simulate production time
    }
}

void* consumer(void* arg) {
    int item;
    while (1) {
        sem_wait(&full);            // Wait for a full slot
        pthread_mutex_lock(&mutex); // Lock the buffer

        item = buffer[out]; // Read item from the buffer
        printf("Consumed: %d\n", item);
        out = (out + 1) % BUFFER_SIZE; // Update read position

        pthread_mutex_unlock(&mutex); // Unlock the buffer
        sem_post(&empty);             // Increment empty-slot semaphore

        sleep(2); // Simulate consumption time
    }
}

int main() {
    pthread_t prod_thread, cons_thread;

    // Initialize semaphores and mutex
    sem_init(&empty, 0,
        BUFFER_SIZE);      // Empty-slot semaphore initialized to buffer size
    sem_init(&full, 0, 0); // Full-slot semaphore initialized to 0
    pthread_mutex_init(&mutex, NULL); // Initialize mutex

    // Create producer and consumer threads
    pthread_create(&prod_thread, NULL, producer, NULL);
    pthread_create(&cons_thread, NULL, consumer, NULL);

    // Wait for threads to finish
    pthread_join(prod_thread, NULL);
    pthread_join(cons_thread, NULL);

    // Destroy semaphores and mutex
    sem_destroy(&empty);
    sem_destroy(&full);
    pthread_mutex_destroy(&mutex);

    return 0;
}
