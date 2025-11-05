/*
int pthread_cancel(pthread_t thread);
thread: The thread ID (of type pthread_t) of the target thread to cancel.​
The function returns 0 on success; otherwise, it returns an error code such as ESRCH if no thread with that ID exists.​
*/

#include <pthread.h>
#include <stdio.h>
#include <unistd.h>


void* th_function(void* n) {
    for (int sec = 0; sec < 10; ++sec) {
        printf("Seconds from init thread %i\n", sec);
        pthread_testcancel(); // Cancellation point
        sleep(1);
    }
    return NULL;
}

int main() {
    pthread_t thread1;

    pthread_create(&thread1, NULL, th_function, NULL);
    sleep(5);              // Let the thread run for 5 seconds
    pthread_cancel(thread1); // Request cancellation
    pthread_join(thread1, NULL); // Wait for thread to end

    printf("Main thread finished.\n");
    return 0;
}
