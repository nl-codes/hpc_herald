/****************************************************************************
   busy_wait_spin.c

  Demonstrates ordered mutual exclusion using busy waiting (“spinning”).
  Threads spin on a shared flag and only enter the critical section when their turn comes.
  Each thread also counts its own spins (iterations of the busy-wait loop)
  to show how much CPU time is spent waiting.

  Compile with:
    gcc -pthread busy_wait_spin.c -o busy_wait_spin

  Run with:
    ./busy_wait_spin

  Things to note:
    - Each thread waits (spins) until flag == its thread rank, ensuring only one thread
      enters the critical section at a time.
    - Threads increment the shared counter in strict order (0, 1, ...).
    - The 'spin' variable records how many times each thread loops in its busy-wait,
      giving a quantitative measure of wasted CPU cycles.
    - This approach prevents race conditions, but demonstrates how busy-wait loops
      can waste significant CPU resources, especially as thread counts rise or contention increases.
****************************************************************************/

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

#define NUM_THREADS 3
#define ROUNDS 10

volatile int counter = 0;   /* Shared counter variable */
volatile int flag = 0;      /* Shared flag to order threads */
volatile long total_spin = 0;

void* worker(void* arg) {
  int my_rank = *(int*)arg;
  long spins = 0;
  for (int round = 0; round < ROUNDS; round++) {
    /* Each thread busy waits (spins) until it's their turn */
    while (flag != my_rank) {
      /* Do nothing -- busy waiting wastes CPU cycles! */
      spins++;
    }
    printf("Thread %d spin %ld times\n", my_rank, spins);
    /* Critical section: only thread with flag == my_rank may enter */
    counter++;
    printf("Thread %d incremented counter to %d\n", my_rank, counter);
    total_spin += spins;
    flag = (flag + 1) % NUM_THREADS; /* Pass turn to next thread */
  }

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

  printf("Final counter: %d (Expected: %d)\n", counter, NUM_THREADS * ROUNDS);
  printf("Final spin: %ld\n", total_spin);
  /*
   * This time, the counter is always correct (no race condition) because
   * each thread enters the critical section in strict order, one by one.
   * However, while waiting, all threads waste CPU cycles repeatedly checking
   * the flag. This is the downside of busy-waiting for mutual exclusion.
   */
  return 0;
}
