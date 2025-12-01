#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>

char* messages[3] = { NULL, NULL, NULL };
omp_lock_t locks[3];  // One lock per thread

int main() {
    double start = omp_get_wtime();

    // Initialize all locks
    for (int i = 0; i < 3; i++) {
        omp_init_lock(&locks[i]);
    }

    // Lock all threads except thread 0
    for (int i = 1; i < 3; i++) {
        omp_set_lock(&locks[i]);
    }

#pragma omp parallel num_threads(3) default(none) shared(messages, locks)
    {
        int tid = omp_get_thread_num();
        int nthreads = omp_get_num_threads();
        char tmpbuf[100];
        int dest = (tid + 1) % nthreads;

        for (int i = 0; i < 10; i++) {
            // Wait for this thread's turn to execute
            omp_set_lock(&locks[tid]);

            // RECEIVE phase (skip for thread 0 in first iteration)
            if (!(i == 0 && tid == 0)) {
                printf("Thread %d received the message '%s'\n", tid, messages[tid]);
                free(messages[tid]);
                messages[tid] = NULL;
            }

            // SEND phase
            sprintf(tmpbuf, "Hello from Thread %d!", tid);
            char* msg = strdup(tmpbuf);
            messages[dest] = msg;
            printf("Thread %d sent the message to Thread %d\n", tid, dest);

            // Release next thread
            omp_unset_lock(&locks[dest]);
        }
    }

    // Destroy locks
    for (int i = 0; i < 3; i++) {
        omp_destroy_lock(&locks[i]);
    }

    double end = omp_get_wtime();
    printf("Time = %f seconds\n", end - start);

    return 0;
}
