/****************************************************************************
  race_condition_addition_userinput.c

  Illustrates race condition with concurrent addition. Now accepts
  thread count from user.

  Compile with:
    gcc -pthread race_condition_addition.c -o race_condition_addition

  Run with:
    ./race_condition_addition

  Things to note:
    - Prompts user for number of threads.
    - Each thread adds 5 to the shared total.
    - Race condition may cause loss of some additions. Total is
      sometimes less than expected (expected: threads * 5).
****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

volatile int total = 0; /* Shared variable; prone to race condition */

void* add_five(void* arg) {
  int temp = total;        /* Each thread reads the shared total */
  usleep(20);              /* Simulate some work */
  temp += 5;               /* Adds 5 in its local register */
  total = temp;            /* Writes result back */
  return NULL;
}

int main() {
  int thread_no;
  printf("Enter number of threads: ");
  scanf("%d", &thread_no);

  pthread_t* threads = malloc(thread_no * sizeof(pthread_t));
  if (!threads) {
    fprintf(stderr, "Memory allocation failed.\n");
    return 1;
  }

  /* Create threads */
  for (int i = 0; i < thread_no; ++i) {
    pthread_create(&threads[i], NULL, add_five, NULL);
  }

  /* Wait for all threads to finish */
  for (int i = 0; i < thread_no; ++i) {
    pthread_join(threads[i], NULL);
  }

  printf("Final total: %d (Expected: %d)\n", total, thread_no * 5);
  /*
   * Ideally, the total should be thread_no * 5. However, due to the race condition,
   * you will often get a lower value, especially with more threads. Run this
   * program several times (and try with higher thread counts) to see the effect.
   * Each thread may overwrite others' results if they run concurrently.
   */

  free(threads);
  return 0;
}
