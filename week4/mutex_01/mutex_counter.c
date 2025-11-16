/****************************************************************************
  mutex_counter.c

  Demonstrates mutual exclusion (mutex) for protecting a critical section.
  Multiple threads increment a shared counter. The mutex ensures only one
  thread can update the counter at a time, preventing race conditions.

  Compile with:
    gcc -pthread mutex_counter.c -o mutex_counter

  Run with:
    ./mutex_counter

  Things to note:
    - pthread_mutex_t is used as a lock.
    - Each thread acquires the lock before incrementing the counter and releases it after.
    - Result is always correct (final value = NUM_THREADS * NUM_INCREMENTS), unlike with unsafe code or busy waiting alone.
****************************************************************************/

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

#define NUM_THREADS 4
#define NUM_INCREMENTS 100000

int counter = 0;                /* Shared counter variable */
pthread_mutex_t mutex;          /* Mutex to protect the counter */

void* increment(void* arg) {
  for (int i = 0; i < NUM_INCREMENTS; ++i) {
    pthread_mutex_lock(&mutex);     // Lock before entering critical section
    counter++;                      // Critical section
    pthread_mutex_unlock(&mutex);   // Unlock after leaving critical section
  }
  return NULL;
}

int main() {
  pthread_t threads[NUM_THREADS];

  pthread_mutex_init(&mutex, NULL);   /* Initialize the mutex */

  for (int i = 0; i < NUM_THREADS; ++i) {
    pthread_create(&threads[i], NULL, increment, NULL);
  }
  for (int i = 0; i < NUM_THREADS; ++i) {
    pthread_join(threads[i], NULL);
  }

  printf("Final counter value: %d (Expected: %d)\n", counter, NUM_THREADS * NUM_INCREMENTS);

  pthread_mutex_destroy(&mutex);      /* Destroy the mutex */
  return 0;
}
