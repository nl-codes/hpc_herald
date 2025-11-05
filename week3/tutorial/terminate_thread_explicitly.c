/*
A thread can terminate itself by calling pthread_exit(void *retval);.
This immediately ends the calling thread and optionally makes a return value available to other threads that might call pthread_join() to collect the thread's exit status.
*/


#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void* thread_func(void* arg) {
    char* message = malloc(32);
    if (message == NULL) {
        perror("Failed to allocate memory");
        pthread_exit(NULL);
    }
    strcpy(message, "Thread finished with exit.");
    pthread_exit(message); // Explicitly terminates the thread and returns 'message'
}

int main() {
    pthread_t tid;
    void* exit_status;

    if (pthread_create(&tid, NULL, thread_func, NULL) != 0) {
        perror("pthread_create() error");
        exit(1);
    }

    // Wait for the thread to terminate and collect its exit status
    pthread_join(tid, &exit_status);

    if (exit_status != NULL) {
        printf("Thread exited with message: '%s'\n", (char*)exit_status);
        free(exit_status);
    } else {
        printf("Thread returned no message.\n");
    }

    return 0;
}
