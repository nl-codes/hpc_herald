#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>

#define NUM_THREADS 3

sem_t sema_locker[NUM_THREADS];

char* messages[3] = { NULL, NULL, NULL };

void* messenger(void* p) {
    long tid = (long)p;
    char tmpbuf[100];

    for (int i = 0; i < 10; i++) {
        sem_wait(&sema_locker[tid]);

        if (!(i == 0 && tid == 0)) {
            /* Receiving a message */
            while (messages[tid] == NULL) {}
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
        sem_post(&sema_locker[next]);
    }
    return NULL;
}

int main() {
    pthread_t threads[NUM_THREADS];

    sem_init(&sema_locker[0], 0, 1);
    sem_init(&sema_locker[1], 0, 0);
    sem_init(&sema_locker[2], 0, 0);

    // Thread creation
    for (long i = 0; i < NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, messenger, (void*)i);
    }

    // Thread join
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    // Destroy Semaphore
    for (int i = 0; i < NUM_THREADS; i++)
        sem_destroy(&sema_locker[i]);
    return 0;
}
