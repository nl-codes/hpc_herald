#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>

int HIGHEST = 100;
int THREADS;

// Function to check if a number is prime
int is_prime(int n) {
    if (n < 2) return 0;
    for (int i = 2; i <= sqrt(n); i++)
        if (n % i == 0) return 0;
    return 1;
}

// Thread function
void* print_primes(void* args) {
    int t_id = *(int*)args;

    int start = t_id * (HIGHEST / THREADS) + 1;
    int end = (t_id == THREADS - 1) ? HIGHEST : start + (HIGHEST / THREADS) - 1;

    printf("Thread %d handling range %d - %d\n", t_id + 1, start, end);

    for (int i = start; i <= end; i++) {
        if (is_prime(i))
            printf("Thread %d -> %d\n", t_id + 1, i);
    }
    return NULL;
}

int main() {
    printf("Enter the number of threads: ");
    scanf("%d", &THREADS);

    if (THREADS <= 0) {
        printf("Invalid input. Try again.\n");
        return 1;
    }

    pthread_t* threads = malloc(THREADS * sizeof(pthread_t));
    int* thread_ids = malloc(THREADS * sizeof(int));

    for (int i = 0; i < THREADS; i++) {
        thread_ids[i] = i;
        pthread_create(&threads[i], NULL, print_primes, &thread_ids[i]);
    }

    for (int i = 0; i < THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("\nAll threads finished.\n");

    free(threads);
    free(thread_ids);
    return 0;
}
