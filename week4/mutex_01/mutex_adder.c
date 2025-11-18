/****************************************************************************
  mutex_adder.c

  Demonstrates thread-safe addition with two threads and two mutexes.
  Each thread needs both mutexes to update two shared variables,
  but always locks them in the same order so deadlock cannot occur.

  Compile with:
    gcc -pthread mutex_adder.c -o mutex_adder

  Run with:
    ./mutex_adder

  Things to note:
    - Two shared variables (`sum1` and `sum2`), protected by `mutex1` and `mutex2`.
    - Both threads lock `mutex1` first, then `mutex2`, then update both sums.
    - By locking in the same order, circular wait is impossible, so there is no deadlock.
    - The final output is always correct, and the program never hangs.
****************************************************************************/
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

#define THREAD_ITERS 5
#define ADD_VALUE_1    3
#define ADD_VALUE_2    5

int sum1 = 0, sum2 = 0;
pthread_mutex_t mutex1, mutex2;

void* adder1(void* arg) {
  for (int i = 0; i < THREAD_ITERS; ++i) {
    pthread_mutex_lock(&mutex1);
    printf("Thread %ld locked mutex1\n", pthread_self());
    pthread_mutex_lock(&mutex2);
    printf("Thread %ld locked mutex2\n", pthread_self());

    printf("Thread %ld Entered critical section\n", pthread_self());
    sum1 += ADD_VALUE_1;  // Update first shared resource
    sum2 += ADD_VALUE_2;  // Update second shared resource

    pthread_mutex_unlock(&mutex2);
    printf("Thread %ld unlocked mutex2\n", pthread_self());
    pthread_mutex_unlock(&mutex1);
    printf("Thread %ld unlocked mutex1\n", pthread_self());
  }
  return NULL;
}
void* adder2(void* arg) {
  for (int i = 0; i < THREAD_ITERS; ++i) {
    pthread_mutex_lock(&mutex1);
    printf("Thread %ld locked mutex1\n", pthread_self());
    pthread_mutex_lock(&mutex2);
    printf("Thread %ld locked mutex2\n", pthread_self());

    printf("Thread %ld Entered critical section\n", pthread_self());
    sum1 += ADD_VALUE_1;  // Update first shared resource
    sum2 += ADD_VALUE_2;  // Update second shared resource

    pthread_mutex_unlock(&mutex2);
    printf("Thread %ld unlocked mutex2\n", pthread_self());
    pthread_mutex_unlock(&mutex1);
    printf("Thread %ld unlocked mutex1\n", pthread_self());
  }
  return NULL;
}

int main() {
  pthread_t t1, t2;
  pthread_mutex_init(&mutex1, NULL);
  pthread_mutex_init(&mutex2, NULL);

  pthread_create(&t1, NULL, adder1, NULL);
  pthread_create(&t2, NULL, adder2, NULL);

  pthread_join(t1, NULL);
  pthread_join(t2, NULL);

  printf("sum1 = %d (Expected: %d)\n", sum1, 2 * THREAD_ITERS * ADD_VALUE_1);
  printf("sum2 = %d (Expected: %d)\n", sum2, 2 * THREAD_ITERS * ADD_VALUE_2);

  pthread_mutex_destroy(&mutex1);
  pthread_mutex_destroy(&mutex2);
  return 0;
}
