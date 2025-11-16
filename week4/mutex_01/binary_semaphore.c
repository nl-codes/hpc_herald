/****************************************************************************
  binary_semaphore.c
  Demonstrates use of POSIX semaphores for synchronization.
  Two threads increment a shared counter, but access is controlled by a binary semaphore.

  Compile with:
    gcc -pthread binary_semaphore.c -o binary_semaphore

  Run with:
    ./semaphore_demo

  Things to note:
    - Uses sem_t variable, initialized to 1 (acts as a mutex).
    - Threads do sem_wait before accessing shared counter and sem_post afterward.
    - At most one thread can access the counter at a time, preventing race conditions.
****************************************************************************/
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

#define NUM_INCREMENTS 100000

int counter = 0;
sem_t sem;

void* add_counter(void* arg) {
    for (int i = 0; i < NUM_INCREMENTS; ++i) {
        sem_wait(&sem);      // Wait/decrement: enter critical section
        counter++;           // Only one thread here at a time!
        sem_post(&sem);      // Signal/increment: leave critical section
    }
    return NULL;
}

int main() {
    pthread_t t1, t2;

    sem_init(&sem, 0, 1); // Binary semaphore for mutual exclusion (0=local, 1=initial value)

    pthread_create(&t1, NULL, add_counter, NULL);
    pthread_create(&t2, NULL, add_counter, NULL);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    printf("Final counter value: %d (Expected: %d)\n", counter, 2 * NUM_INCREMENTS);

    sem_destroy(&sem); // Cleanup
    return 0;
}
