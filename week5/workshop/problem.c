#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

char* messages[3] = { NULL, NULL, NULL };

void* messenger(void* p) {
    long tid = (long)p;
    char tmpbuf[100];

    for (int i = 0; i < 10; i++) {
        /* Sending a message */
        long int dest = (tid + 1) % 3;
        sprintf(tmpbuf, "Hello from Thread %ld!", tid);
        char* msg = strdup(tmpbuf);
        messages[dest] = msg;
        printf("Thread %ld sent the message to Thread %ld\n", tid, dest);

        /* Receiving a message */
        printf("Thread %ld received the message '%s'\n", tid, messages[tid]);
        free(messages[tid]);
        messages[tid] = NULL;
    }
    return NULL;
}

int main() {
    pthread_t thrID1, thrID2, thrID3;

    pthread_create(&thrID1, NULL, messenger, (void*)0);
    pthread_create(&thrID2, NULL, messenger, (void*)1);
    pthread_create(&thrID3, NULL, messenger, (void*)2);
    pthread_join(thrID1, NULL);
    pthread_join(thrID2, NULL);
    pthread_join(thrID3, NULL);

    return 0;
}
