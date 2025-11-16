/****************************************************************************
  semaphore_resource_pool.c

  Demonstrates using a counting semaphore to manage a fixed-size resource pool.
  Only N threads can use the resource simultaneously; additional threads wait.

  Compile with:
    gcc -pthread semaphore_resource_pool.c -o semaphore_resource_pool

  Run with:
    ./semaphore_resource_pool

  Things to note:
    - The semaphore is initialized to POOL_SIZE (N).
    - Each thread simulates "using" the resource, then releases it.
    - printf() logs show only N threads enter the pool at a time.
****************************************************************************/
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define NUM_THREADS 10
#define POOL_SIZE   3  // Number of simultaneous "connections"

sem_t resource_sem;

void* worker(void* arg) {
    int id = *(int*)arg;
    printf("Thread %d: waiting for resource...\n", id);

    sem_wait(&resource_sem);  // Wait for a free slot in the pool
    printf("Thread %d: acquired resource.\n", id);

    sleep(2);                 // Simulate using resource (connection)
    printf("Thread %d: releasing resource.\n", id);

    sem_post(&resource_sem);  // Release resource
    return NULL;
}

int main() {
    pthread_t threads[NUM_THREADS];
    int ids[NUM_THREADS];

    // Initialize a counting semaphore to POOL_SIZE
    sem_init(&resource_sem, 0, POOL_SIZE);

    for (int i = 0; i < NUM_THREADS; ++i) {
        ids[i] = i + 1;
        pthread_create(&threads[i], NULL, worker, &ids[i]);
    }
    for (int i = 0; i < NUM_THREADS; ++i) {
        pthread_join(threads[i], NULL);
    }

    sem_destroy(&resource_sem);
    printf("All threads are done.\n");
    return 0;
}
