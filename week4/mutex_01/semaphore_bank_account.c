/****************************************************************************
  semaphore_bank_account.c

  Demonstrates using a POSIX semaphore for safe updates to a shared bank account
  using 1000 threads. Each thread adds 1 penny, so the final account should be 1000p.
  The program measures wall-clock time for the entire run, using a local
  time_difference() function—no external headers required.

  Compile with:
    gcc -pthread semaphore_bank_account.c -o semaphore_bank_account

  Run with:
    ./semaphore_bank_account

  Things to note:
    - Uses sem_t semaphore initialized to 1 for mutual exclusion.
    - Every thread enters critical section safely and adds to the shared account.
    - Demonstrates effective thread synchronization and correct accumulation.
    - Includes local function to measure elapsed time in nanoseconds.

****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>

sem_t sem;

// Calculate the time difference in nanoseconds
void time_difference(struct timespec *start, struct timespec *finish, long long int *difference) {
    *difference = (finish->tv_sec - start->tv_sec) * 1000000000LL + (finish->tv_nsec - start->tv_nsec);
}

void *add_penny(void *bal) {
    int *balance = bal;

    sem_wait(&sem); // Enter critical section
    int b = *balance;
    // Cause a short delay
    int i;
    for (i = 0; i < 100000; i++) {}
    b = b + 1;
    *balance = b;
    sem_post(&sem); // Leave critical section

    return NULL;
}

int main() {
    struct timespec start, finish;
    long long int difference;
    int account = 0;
    int i;
    int n = 1000;
    int result;

    sem_init(&sem, 0, 1); // Binary semaphore

    clock_gettime(CLOCK_MONOTONIC, &start);

    pthread_t t[n];
    for (i = 0; i < n; i++) {
        result = pthread_create(&t[i], NULL, add_penny, &account);
        if (result != 0) {
            printf("thread %d failed to start\n", i);
            printf("try lowering stack size with ulimit -s 1024\n");
            printf("to find current limit: ulimit -a \n");
            exit(1);
        }
    }
    for (i = 0; i < n; i++) {
        pthread_join(t[i], NULL);
    }

    clock_gettime(CLOCK_MONOTONIC, &finish);
    time_difference(&start, &finish, &difference);
    printf("accumulated %dp\n", account);
    printf("run lasted %lldns or %9.5lfs\n", difference, difference / 1000000000.0);

    sem_destroy(&sem);
    return 0;
}
