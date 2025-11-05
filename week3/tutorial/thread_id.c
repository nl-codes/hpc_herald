/*
pthread_t pthread_self(void);
This function returns the thread ID of the calling thread as a value of type pthread_t.
*/

#include <pthread.h>
#include <stdio.h>
#include <windows.h>

void* thread_func(void* arg) {
    pthread_t tid = pthread_self();
    printf("Thread ID in thread_func: %lu\n", (unsigned long)tid);
    // printf("Thread ID in thread_func: %lu\n", GetCurrentThreadId());
    pthread_exit(NULL);
}

int main() {
    pthread_t tid;

    pthread_create(&tid, NULL, thread_func, NULL);
    printf("Thread ID in main: %lu\n", (unsigned long)tid);
    // printf("Thread ID in main: %lu\n", GetCurrentThreadId());

    pthread_join(tid, NULL);
    return 0;
}
