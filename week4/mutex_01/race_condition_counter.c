/****************************************************************************
  race_condition_counter.c

  Demonstrates a race condition using counter increment by multiple threads.
  Two threads each increment a shared counter many times without synchronization.
  Due to a race condition (both threads reading/modifying/writing simultaneously),
  the final result is often less than expected.

  Compile with:

    gcc -pthread race_condition_counter.c -o race_condition_counter

  Run with:

    ./race_condition_counter

  Things to note:
   - The counter variable is shared between threads.
   - Each thread increments the counter 100,000 times.
   - 'counter++' is not atomic, so increments may be lost when threads overlap.
   - Expected final value is 200000, but output will usually be less, and
     can differ on each run. This demonstrates a classic race condition.
****************************************************************************/

#include <stdio.h>
#include <pthread.h>

#define NUM_THREADS 3
#define NUM_INCREMENTS 100000

int counter = 0; /* Shared counter variable; not protected from race condition */

void* increment(void* arg) {
  int i;
  for (i = 0; i < NUM_INCREMENTS; ++i) {
    counter++; /* Critical section: this is NOT atomic and causes race condition */
  }
  return NULL;
}

int main() {
  pthread_t threads[NUM_THREADS];
  int i;

  /* Create threads that increment the shared counter */
  for (i = 0; i < NUM_THREADS; ++i) {
    pthread_create(&threads[i], NULL, increment, NULL);
  }

  /* Wait for threads to finish */
  for (i = 0; i < NUM_THREADS; ++i) {
    pthread_join(threads[i], NULL);
  }

  printf("Final counter value: %d (Expected: %d)\n", counter, NUM_THREADS * NUM_INCREMENTS);
  /*
   * Expected output: Final counter value: 200000 (Expected: 200000)
   * Actual output: Usually LESS than 200000, e.g., 187543, 169212, etc.
   * Why? Because 'counter++' can be interrupted—threads may overwrite
   * each other's updates, so increments are lost. Try running multiple times
   * to see the value change. This is the race condition!
   */

  return 0;
}
