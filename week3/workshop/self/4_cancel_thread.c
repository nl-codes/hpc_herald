#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>

int HIGHEST = 100;
int THREADS_SIZE;
int total_primes_found = 0;      // global counter
pthread_mutex_t lock;            // mutex for synchronization
pthread_t* threads;              // to allow global cancellation

// Function to check if a number is prime
int is_prime(int n) {
    if (n < 2) return 0;
    for (int i = 2; i <= sqrt(n); i++)
        if (n % i == 0) return 0;
    return 1;
}

// Thread function
void* find_primes(void* args) {
    int t_id = *(int*)args;

    int start = t_id * (HIGHEST / THREADS_SIZE) + 1;
    int end = (t_id == THREADS_SIZE - 1) ? HIGHEST : start + (HIGHEST / THREADS_SIZE) - 1;

    printf("Thread %d handling range %d - %d\n", t_id + 1, start, end);

    for (int i = start; i <= end; i++) {
        // Allow thread to respond to cancellation requests
        pthread_testcancel();

        if (is_prime(i)) {
            pthread_mutex_lock(&lock);

            if (total_primes_found < 5) {
                total_primes_found++;
                printf("Thread %d -> Found prime #%d: %d\n", t_id + 1, total_primes_found, i);
            }

            // If 5th prime found, cancel all threads
            if (total_primes_found >= 5) {
                for (int j = 0; j < THREADS_SIZE; j++) {
                    if (threads[j] != pthread_self()) {
                        pthread_cancel(threads[j]);
                    }
                }
                pthread_mutex_unlock(&lock);
                pthread_exit(NULL);
            }

            pthread_mutex_unlock(&lock);
        }
    }

    pthread_exit(NULL);
}

int main() {
    printf("Enter the number of threads: ");
    scanf("%d", &THREADS_SIZE);

    if (THREADS_SIZE <= 0) {
        printf("Invalid input. Try again.\n");
        return 1;
    }

    threads = malloc(THREADS_SIZE * sizeof(pthread_t));
    int* thread_ids = malloc(THREADS_SIZE * sizeof(int));

    pthread_mutex_init(&lock, NULL);

    for (int i = 0; i < THREADS_SIZE; i++) {
        thread_ids[i] = i;
        pthread_create(&threads[i], NULL, find_primes, &thread_ids[i]);
    }

    for (int i = 0; i < THREADS_SIZE; i++) {
        pthread_join(threads[i], NULL);
    }

    pthread_mutex_destroy(&lock);
    free(threads);
    free(thread_ids);

    printf("\nProgram finished after finding 5 primes.\n");
    return 0;
}
