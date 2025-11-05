#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <windows.h> // for DWORD

DWORD core_id;
void* task(void* arg) {
    int thread_id = *(int*)arg;

    // Get current CPU core ID
    int cpu = GetCurrentProcessorNumber();

    printf("Thread %d is running on CPU core %d\n", thread_id, cpu);

    for (int i = 0; i < 5; i++) {
        printf("Thread %d - Step %d\n", thread_id, i + 1);
        sleep(1);
    }

    return NULL;
}

int main() {
    pthread_t threads[4];
    int ids[4];

    for (int i = 0; i < 4; i++) {
        ids[i] = i;
        pthread_create(&threads[i], NULL, task, &ids[i]);
    }

    for (int i = 0; i < 4; i++) {
        pthread_join(threads[i], NULL);
    }

    return 0;
}
