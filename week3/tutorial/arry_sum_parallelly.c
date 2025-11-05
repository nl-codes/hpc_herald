#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>

#define SIZE 1000000000
#define THREADS 4

int *array;
long long partial_sum[THREADS];

void* thread_sum(void* arg) {
    int tid = *(int*)arg;
    long long sum = 0;
    int start = tid * (SIZE / THREADS);
    int end = (tid == THREADS - 1) ? SIZE : start + (SIZE / THREADS);

    for (int i = start; i < end; i++)
        sum += array[i];
    partial_sum[tid] = sum;
    return NULL;
}

int main() {
    array = malloc(SIZE * sizeof(int));
    pthread_t threads[THREADS];
    int thread_ids[THREADS];
    struct timeval start, end;
    double elapsed;
    long long total_sum = 0;

    for (int i = 0; i < SIZE; i++)
        array[i] = 1;

    gettimeofday(&start, NULL);

    for (int i = 0; i < THREADS; i++) {
        thread_ids[i] = i;
        pthread_create(&threads[i], NULL, thread_sum, &thread_ids[i]);
    }
    for (int i = 0; i < THREADS; i++)
        pthread_join(threads[i], NULL);

    for (int i = 0; i < THREADS; i++)
        total_sum += partial_sum[i];

    gettimeofday(&end, NULL);

    // print start and end times
    printf("Start time: %ld.%06ld seconds\n", start.tv_sec, start.tv_usec);
    printf("End time  : %ld.%06ld seconds\n", end.tv_sec, end.tv_usec);

    elapsed = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1e6;
    printf("Multi-threaded sum: %lld\n", total_sum);
    printf("Time taken (secs): %f\n", elapsed);

    free(array);
    return 0;
}
