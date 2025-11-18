#include <stdio.h>
#include <pthread.h>

typedef struct {
    int accountNumber;
    double balance;
} Account;

Account accounts[10];

pthread_mutex_t withdraw_locker, deposit_locker;

void* withdraw(void* p) {
    int accountId = *(int*)p;
    double amount = 100;  // Withdraw amount

    pthread_mutex_lock(&withdraw_locker);
    accounts[accountId].balance -= amount;
    pthread_mutex_unlock(&withdraw_locker);

    return NULL;
}

void* deposit(void* p) {
    int accountId = *(int*)p;
    double amount = 100;  // Deposit amount

    pthread_mutex_lock(&withdraw_locker);
    accounts[accountId].balance += amount;
    pthread_mutex_unlock(&withdraw_locker);

    return NULL;
}

int main() {
    pthread_t threads[20];
    int ids[10];

    pthread_mutex_init(&withdraw_locker, NULL);
    pthread_mutex_init(&deposit_locker, NULL);
    // Create multiple threads to simulate transactions on the same account
    for (int i = 0; i < 10; i++) {
        ids[i] = i;
        pthread_create(&threads[i], NULL, withdraw, &ids[i]);
        pthread_create(&threads[i + 10], NULL, deposit, &ids[i]);
    }
    for (int i = 0; i < 20; i++) {
        pthread_join(threads[i], NULL);
    }
    // Print final balance
    for (int i = 0; i < 10; i++) {
        printf("Account %d balance = %.2f\n", i, accounts[i].balance);
    }

    pthread_mutex_destroy(&withdraw_locker);
    pthread_mutex_destroy(&deposit_locker);

    return 0;
}
