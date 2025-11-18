#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

sem_t printerSema;

void* printer(void* p) {
    int userId = *(int*)p;

    printf("User %d is waiting for a printer...\n", userId);

    // Wait until a printer becomes free
    sem_wait(&printerSema);

    // Printer acquired
    printf("User %d has finished printing.\n", userId);

    // Release printer
    sem_post(&printerSema);

    return NULL;
}

int main() {
    int num_users = 10;

    pthread_t users[num_users];
    int ids[num_users];

    // Initialize semaphore with value 2 : two printers available
    sem_init(&printerSema, 0, 2);

    // Create user threads
    for (int i = 0; i < num_users; i++) {
        ids[i] = i;
        pthread_create(&users[i], NULL, printer, &ids[i]);
    }

    // Wait for all users to finish
    for (int i = 0; i < num_users; i++) {
        pthread_join(users[i], NULL);
    }

    sem_destroy(&printerSema);

    return 0;
}
