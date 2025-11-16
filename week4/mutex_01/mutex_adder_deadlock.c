/****************************************************************************
  mutex_adder_deadlock.c

  Demonstrates deadlock: two threads using two mutexes, but each locks the mutexes
  in a different order, and both pause holding the first mutex—so deadlock always occurs.

  Compile with:
    

  Run with:
    ./mutex_adder_deadlock

  Things to note:
    - Two shared variables, `sum1` and `sum2`, protected by `mutex1` and `mutex2`.
    - Thread 1 locks mutex1, sleeps, then tries to lock mutex2.
    - Thread 2 locks mutex2, sleeps, then tries to lock mutex1.
    - Both threads grab their first lock, then wait for the other lock—which is already held.
      No thread can proceed, so the program always hangs: this is deadlock!
****************************************************************************/
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

#define THREAD_ITERS 5000
#define ADD_VALUE_1  3
#define ADD_VALUE_2  5

int sum1 = 0, sum2 = 0;
pthread_mutex_t mutex1, mutex2;

void* adder1(void* arg) {
    for (int i = 0; i < THREAD_ITERS; ++i) {
        pthread_mutex_lock(&mutex1);
        printf("Thread %ld locked mutex1\n", pthread_self());
        usleep(1000); // Force a pause to increase overlap
        pthread_mutex_lock(&mutex2);
        printf("Thread %ld locked mutex2\n", pthread_self());

        printf("Thread %ld Entered critical section\n", pthread_self());
        sum1 += ADD_VALUE_1;
        sum2 += ADD_VALUE_2;

        pthread_mutex_unlock(&mutex2);
        printf("Thread %ld unlocked mutex2\n", pthread_self());
        pthread_mutex_unlock(&mutex1);
        printf("Thread %ld unlocked mutex1\n", pthread_self());
    }
    return NULL;
}

void* adder2(void* arg) {
    for (int i = 0; i < THREAD_ITERS; ++i) {
        pthread_mutex_lock(&mutex2);
        printf("Thread %ld locked mutex2\n", pthread_self());
        usleep(1000); // Force a pause to increase overlap
        pthread_mutex_lock(&mutex1);
        printf("Thread %ld locked mutex1\n", pthread_self());

        printf("Thread %ld Entered critical section\n", pthread_self());
        sum1 += ADD_VALUE_1;
        sum2 += ADD_VALUE_2;

        pthread_mutex_unlock(&mutex1);
        printf("Thread %ld unlocked mutex1\n", pthread_self());
        pthread_mutex_unlock(&mutex2);
        printf("Thread %ld unlocked mutex2\n", pthread_self());
    }
    return NULL;
}

int main() {
    pthread_t t1, t2;
    pthread_mutex_init(&mutex1, NULL);
    pthread_mutex_init(&mutex2, NULL);

    pthread_create(&t1, NULL, adder1, NULL);
    pthread_create(&t2, NULL, adder2, NULL);

    pthread_join(t1, NULL); // Neither of these will ever finish...
    pthread_join(t2, NULL);

    // The following will never be reached if deadlock occurs as expected
    printf("sum1 = %d\n", sum1);
    printf("sum2 = %d\n", sum2);

    pthread_mutex_destroy(&mutex1);
    pthread_mutex_destroy(&mutex2);
    return 0;
}
