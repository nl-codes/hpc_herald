#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

void* countNumbers(void* p) {
    for (int i = 0; i < 10; i++) {
        printf("Thread ID %ld: i=%d\n", pthread_self(), i);
        usleep(1000);
    }
}
int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Usage: %s <number_of_threads>\n", argv[0]);
        return 1;
    }

    int numThreads = atoi(argv[1]);
    if (numThreads <= 0) {
        printf("Please enter a valid positive number of threads.\n");
        return 1;
    }

    pthread_t* threads = malloc(numThreads * sizeof(pthread_t));

    if (threads == NULL) {
        printf("Error on memory allocation!\n");
        return 1;
    }

    for (int i = 0; i < numThreads; i++) {
        pthread_create(&threads[i], NULL, countNumbers, NULL);
    }

    for (int i = 0; i < numThreads; i++) {
        pthread_join(threads[i], NULL);
    }

    free(threads);
    return 0;
}
