#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#define SIZE 1000000000

int main() {
    int *array = malloc(SIZE * sizeof(int));
    long long sum = 0;
    struct timeval start, end;
    double elapsed;

    for (int i = 0; i < SIZE; i++)
        array[i] = 1;

    gettimeofday(&start, NULL);
    
    for (int i = 0; i < SIZE; i++)
        sum += array[i];
    gettimeofday(&end, NULL);
    
        // print start and end times
    printf("Start time: %ld.%06ld seconds\n", start.tv_sec, start.tv_usec);
    printf("End time  : %ld.%06ld seconds\n", end.tv_sec, end.tv_usec);

    elapsed = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1e6;
    printf("Single-threaded sum: %lld\n", sum);
    printf("Time taken (secs): %f\n", elapsed);

    free(array);
    return 0;
}
