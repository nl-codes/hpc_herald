/****************************************************************************
  mutex_adder_deadlock_avoided_verbose.c

  Demonstrates deadlock avoidance using pthread_mutex_trylock().
  Informative printf() statements show each step: lock attempts, failures,
  work done, and lock release.

  Compile with:
    gcc -pthread mutex_adder_deadlock_avoided_verbose.c -o mutex_adder_deadlock_avoided_verbose

  Run with:
    ./mutex_adder_deadlock_avoided_verbose

  Things to note:
    - Each thread announces when it attempts, acquires, or fails to acquire each lock.
    - If a second lock cannot be acquired, the thread releases the first and retries.
    - Addition and unlock events are shown, so you can follow how deadlock is avoided.
****************************************************************************/
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

#define THREAD_ITERS 5
#define ADD_VALUE_1  3
#define ADD_VALUE_2  5

int sum1 = 0, sum2 = 0;
pthread_mutex_t mutex1, mutex2;

void* adder1(void* arg) {
    for (int i = 0; i < THREAD_ITERS; ++i) {
        while (1) {
            printf("Adder1: Trying to lock mutex1...\n");
            if (pthread_mutex_trylock(&mutex1) == 0) {
                printf("Adder1: Locked mutex1.\n");
                usleep(1000);
                printf("Adder1: Trying to lock mutex2...\n");
                if (pthread_mutex_trylock(&mutex2) == 0) {
                    printf("Adder1: Locked mutex2.\n");
                    sum1 += ADD_VALUE_1;
                    sum2 += ADD_VALUE_2;
                    printf("Adder1: Added to sums (sum1=%d, sum2=%d).\n", sum1, sum2);
                    pthread_mutex_unlock(&mutex2);
                    printf("Adder1: Unlocked mutex2.\n");
                    pthread_mutex_unlock(&mutex1);
                    printf("Adder1: Unlocked mutex1.\n");
                    break;
                } else {
                    printf("Adder1: Failed to lock mutex2. Releasing mutex1 and retrying.\n");
                    pthread_mutex_unlock(&mutex1);
                    usleep(10);
                }
            } else {
                printf("Adder1: Failed to lock mutex1. Retrying.\n");
                usleep(10);
            }
        }
    }
    return NULL;
}

void* adder2(void* arg) {
    for (int i = 0; i < THREAD_ITERS; ++i) {
        while (1) {
            printf("Adder2: Trying to lock mutex2...\n");
            if (pthread_mutex_trylock(&mutex2) == 0) {
                printf("Adder2: Locked mutex2.\n");
                usleep(1000);
                printf("Adder2: Trying to lock mutex1...\n");
                if (pthread_mutex_trylock(&mutex1) == 0) {
                    printf("Adder2: Locked mutex1.\n");
                    sum1 += ADD_VALUE_1;
                    sum2 += ADD_VALUE_2;
                    printf("Adder2: Added to sums (sum1=%d, sum2=%d).\n", sum1, sum2);
                    pthread_mutex_unlock(&mutex1);
                    printf("Adder2: Unlocked mutex1.\n");
                    pthread_mutex_unlock(&mutex2);
                    printf("Adder2: Unlocked mutex2.\n");
                    break;
                } else {
                    printf("Adder2: Failed to lock mutex1. Releasing mutex2 and retrying.\n");
                    pthread_mutex_unlock(&mutex2);
                    usleep(10);
                }
            } else {
                printf("Adder2: Failed to lock mutex2. Retrying.\n");
                usleep(10);
            }
        }
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
