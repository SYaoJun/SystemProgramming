#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define BUFFER_SIZE 5 // 缓冲区的大小

int buffer[BUFFER_SIZE]; // 缓冲区数组
int in  = 0;             // 生产者写入位置
int out = 0;             // 消费者读取位置

sem_t           empty; // 用于表示空缓冲区槽位的信号量
sem_t           full;  // 用于表示满缓冲区槽位的信号量
pthread_mutex_t mutex; // 保护缓冲区的互斥锁

void* producer(void* arg) {
    int item;
    while (1) {
        item = rand() % 100;        // 生成一个随机数作为生产的项目
        sem_wait(&empty);           // 等待空槽位
        pthread_mutex_lock(&mutex); // 锁定缓冲区

        buffer[in] = item; // 将项目写入缓冲区
        printf("Produced: %d\n", item);
        in = (in + 1) % BUFFER_SIZE; // 更新写入位置

        pthread_mutex_unlock(&mutex); // 解锁缓冲区
        sem_post(&full);              // 增加满槽位信号量

        sleep(1); // 模拟生产时间
    }
}

void* consumer(void* arg) {
    int item;
    while (1) {
        sem_wait(&full);            // 等待满槽位
        pthread_mutex_lock(&mutex); // 锁定缓冲区

        item = buffer[out]; // 从缓冲区读取项目
        printf("Consumed: %d\n", item);
        out = (out + 1) % BUFFER_SIZE; // 更新读取位置

        pthread_mutex_unlock(&mutex); // 解锁缓冲区
        sem_post(&empty);             // 增加空槽位信号量

        sleep(2); // 模拟消费时间
    }
}

int main() {
    pthread_t prod_thread, cons_thread;

    // 初始化信号量和互斥锁
    sem_init(&empty, 0, BUFFER_SIZE); // 空槽位信号量初始化为缓冲区大小
    sem_init(&full, 0, 0);            // 满槽位信号量初始化为0
    pthread_mutex_init(&mutex, NULL); // 初始化互斥锁

    // 创建生产者和消费者线程
    pthread_create(&prod_thread, NULL, producer, NULL);
    pthread_create(&cons_thread, NULL, consumer, NULL);

    // 等待线程完成
    pthread_join(prod_thread, NULL);
    pthread_join(cons_thread, NULL);

    // 销毁信号量和互斥锁
    sem_destroy(&empty);
    sem_destroy(&full);
    pthread_mutex_destroy(&mutex);

    return 0;
}
