/****************************************************************************
  busy_wait_ordered.c

  Demonstrates ordered mutual exclusion using busy waiting. Threads spin on a
  shared flag and only enter the critical section when their turn comes.

  Compile with:
    gcc -pthread busy_wait_ordered.c -o busy_wait_ordered

  Run with:
    ./busy_wait_ordered

  Things to note:
    - Each thread waits until flag == its thread rank, ensuring only one thread
      enters the critical section at a time.
    - Threads increment the shared counter in strict order (0, 1, ...).
    - This approach prevents race conditions, but the busy-wait loop can waste CPU.
****************************************************************************/

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

#define NUM_THREADS 3
// #define ROUNDS 10

volatile int counter = 0;   /* Shared counter variable */
volatile int flag = 0;      /* Shared flag to order threads */

void* worker(void* arg) {
  int my_rank = *(int*)arg;
  // for(int round = 0; round < ROUNDS; round++) {
    /* Each thread busy waits (spins) until it's their turn */
  while (flag != my_rank) {
    /* Do nothing -- busy waiting wastes CPU cycles! */
  }
  /* Critical section: only thread with flag == my_rank may enter */
  counter++;
  printf("Thread %d incremented counter to %d\n", my_rank, counter);
  flag = (flag + 1) % NUM_THREADS; /* Pass turn to next thread */
  // }

  return NULL;
}

int main() {
  pthread_t threads[NUM_THREADS];
  int ranks[NUM_THREADS];

  for (int i = 0; i < NUM_THREADS; i++) {
    ranks[i] = i;
    pthread_create(&threads[i], NULL, worker, &ranks[i]);
  }

  for (int i = 0; i < NUM_THREADS; i++) {
    pthread_join(threads[i], NULL);
  }

  printf("Final counter: %d (Expected: %d)\n", counter, NUM_THREADS);
  /*
   * This time, the counter is always correct (no race condition) because
   * each thread enters the critical section in strict order, one by one.
   * However, while waiting, all threads waste CPU cycles repeatedly checking
   * the flag. This is the downside of busy-waiting for mutual exclusion.
   */
  return 0;
}
