#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

#define NUM_THREADS 3

pthread_mutex_t locker[NUM_THREADS];

char* messages[3] = { NULL, NULL, NULL };

void* messenger(void* p) {
    long tid = (long)p;
    char tmpbuf[100];

    for (int i = 0; i < 10; i++) {
        pthread_mutex_lock(&locker[tid]);

        if (!(i == 0 && tid == 0)) {
            /* Receiving a message */
            printf("Thread %ld received the message '%s'\n", tid, messages[tid]);
            free(messages[tid]);
            messages[tid] = NULL;
        }

        /* Sending a message */
        long int dest = (tid + 1) % 3;
        sprintf(tmpbuf, "Hello from Thread %ld!", tid);
        char* msg = strdup(tmpbuf);
        messages[dest] = msg;
        printf("Thread %ld sent the message to Thread %ld\n", tid, dest);

        long next = (tid + 1) % NUM_THREADS;
        pthread_mutex_unlock(&locker[next]);
    }
    return NULL;
}

int main() {
    pthread_t threads[NUM_THREADS];

    // Mutex initialize
    for (int i = 0; i < NUM_THREADS; i++)
        pthread_mutex_init(&locker[i], NULL);

    // Lock all mutexes before execution
    for (int i = 0; i < NUM_THREADS; i++)
        pthread_mutex_lock(&locker[i]);

    // Thread creation
    for (long i = 0; i < NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, messenger, (void*)i);
    }

    // Unlock thread 0 so it starts firstf
    pthread_mutex_unlock(&locker[0]);

    // Thread join and Mutex destroy
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
        pthread_mutex_destroy(&locker[i]);
    }

    return 0;
}
