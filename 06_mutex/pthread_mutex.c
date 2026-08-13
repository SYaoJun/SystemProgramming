#include <pthread.h>
#include <stdio.h>

#define NUM_THREADS 2
#define NUM_INCREMENTS 10000

int             counter = 0;
pthread_mutex_t mutex;

void* increment_counter(void* arg) {
    for (int i = 0; i < NUM_INCREMENTS; ++i) {
        pthread_mutex_lock(&mutex); // 加锁
        counter++;
        pthread_mutex_unlock(&mutex); // 解锁
    }
    return NULL;
}

int main() {
    pthread_t threads[NUM_THREADS];

    // 初始化互斥锁
    pthread_mutex_init(&mutex, NULL);

    for (int i = 0; i < NUM_THREADS; ++i) {
        pthread_create(&threads[i], NULL, increment_counter, NULL);
    }

    for (int i = 0; i < NUM_THREADS; ++i) {
        pthread_join(threads[i], NULL);
    }

    // 销毁互斥锁
    pthread_mutex_destroy(&mutex);

    printf("Final counter value: %d\n", counter);

    return 0;
}
